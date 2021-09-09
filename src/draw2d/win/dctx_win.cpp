/*
 * NAppGUI Cross-platform C SDK
 * 2015-2021 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: dctx_win.cpp
 *
 */

/* Draw context */

#include "dctx.h"
#include "dctx.inl"
#include "dctx_win.inl"
#include "cassert.h"
#include "color.h"
#include "bmath.h"
#include "font.h"
#include "font.inl"
#include "heap.h"
#include "ptr.h"
#include "unicode.h"

#if !defined(__WINDOWS__)
#error This file is only for Windows
#endif

/*---------------------------------------------------------------------------*/

static Gdiplus::Color i_color(const color_t c)
{
    uint8_t r, g, b, a;
    color_get_rgba(c, &r, &g, &b, &a);
    return Gdiplus::Color((BYTE)a, (BYTE)r, (BYTE)g, (BYTE)b);
}

/*---------------------------------------------------------------------------*/

DCtx *dctx_create(void *custom_data)
{
    DCtx *ctx = heap_new0(DCtx);
    ctx->pen = new Gdiplus::Pen((Gdiplus::ARGB)Gdiplus::Color::Black);
    ctx->tbrush = new Gdiplus::SolidBrush((Gdiplus::ARGB)Gdiplus::Color::Black);
    ctx->sbrush = new Gdiplus::SolidBrush((Gdiplus::ARGB)Gdiplus::Color::Black);
    ctx->lbrush = new Gdiplus::LinearGradientBrush(Gdiplus::RectF(0, 0, 1, 1e8f), 0, 0, 0);
    ctx->gradient_matrix = new Gdiplus::Matrix;
    ctx->custom_data = custom_data;
    return ctx;
}

/*---------------------------------------------------------------------------*/

DCtx *dctx_bitmap(const uint32_t width, const uint32_t height, const pixformat_t format)
{
    DCtx *ctx = heap_new0(DCtx);
    Gdiplus::PixelFormat pf = PixelFormatUndefined;
    switch (format) {
    case ekRGB24:
        pf = PixelFormat24bppRGB;
        break;
    case ekRGBA32:
        pf = PixelFormat32bppARGB;
        break;
    case ekGRAY8:
    case ekINDEX1:
    case ekINDEX2:
    case ekINDEX4:
    case ekINDEX8:
    case ekFIMAGE:
    cassert_default();
    }

    ctx->width = width;
    ctx->height = height;
    ctx->format = format;
    ctx->pen = new Gdiplus::Pen((Gdiplus::ARGB)Gdiplus::Color::Black);
    ctx->tbrush = new Gdiplus::SolidBrush((Gdiplus::ARGB)Gdiplus::Color::Black);
    ctx->sbrush = new Gdiplus::SolidBrush((Gdiplus::ARGB)Gdiplus::Color::Black);
    ctx->lbrush = new Gdiplus::LinearGradientBrush(Gdiplus::RectF(0, 0, 1, 1e8), 0, 0, 0);
    ctx->gradient_matrix = new Gdiplus::Matrix;
    ctx->bitmap = new Gdiplus::Bitmap((INT)width, (INT)height, pf);
    ctx->graphics = new Gdiplus::Graphics(ctx->bitmap);
    dctx_init(ctx);
    return ctx;
}

/*---------------------------------------------------------------------------*/

void dctx_destroy(DCtx **ctx)
{
    cassert_no_null(ctx);
    cassert_no_null(*ctx);

    if ((*ctx)->font != NULL)
    {
        font_destroy(&(*ctx)->font);
        delete (*ctx)->ffont;
        delete (*ctx)->ffamily;
    }
    else
    {
        cassert((*ctx)->ffont == NULL);
        cassert((*ctx)->ffamily == NULL);
    }

    if ((*ctx)->fpen != NULL)
        delete (*ctx)->fpen;

    delete (*ctx)->pen;
    delete (*ctx)->tbrush;
    delete (*ctx)->sbrush;
    delete (*ctx)->lbrush;
    delete (*ctx)->gradient_matrix;

    if ((*ctx)->gdi_sbrush != NULL)
    {
        BOOL ok = DeleteObject((*ctx)->gdi_sbrush);
        cassert_unref(ok != 0, ok);
    }

    if ((*ctx)->bitmap != NULL)
    {
        cassert_no_null((*ctx)->graphics);
        (*ctx)->bitmap = NULL;
        delete (*ctx)->graphics;
    }
    else
    {
        cassert((*ctx)->graphics == NULL);
    }

    heap_delete(ctx, DCtx);
}

/*---------------------------------------------------------------------------*/

void dctx_set_gcontext(DCtx *ctx, void *gcontext, const uint32_t width, const uint32_t height, const real32_t offset_x, const real32_t offset_y, const uint32_t background, const bool_t reset)
{
    void **context = (void**)gcontext;
    cassert_no_null(ctx);
    cassert(ctx->graphics == NULL);
    ctx->graphics = (Gdiplus::Graphics*)context[0];
    ctx->hdc = (HDC)context[1];
    ctx->background_color = background;
    ctx->width = width;
    ctx->height = height;
    ctx->offset_x = (Gdiplus::REAL)offset_x;
    ctx->offset_y = (Gdiplus::REAL)offset_y;
    ctx->gdi_mode = FALSE;
    if (reset == TRUE)
        dctx_init(ctx);
}

/*---------------------------------------------------------------------------*/

void dctx_unset_gcontext(DCtx *ctx)
{
    cassert_no_null(ctx);
    cassert(ctx->graphics != NULL);
    ctx->graphics = NULL;
}

/*---------------------------------------------------------------------------*/

void dctx_size(const DCtx *ctx, uint32_t *width, uint32_t *height)
{
    cassert_no_null(ctx);
    ptr_assign(width, ctx->width);
    ptr_assign(height, ctx->height);
}

/*---------------------------------------------------------------------------*/

void dctx_transform(DCtx *ctx, const T2Df *t2d, const bool_t cartesian)
{
    cassert_no_null(ctx);
    cassert_no_null(ctx->graphics);
    cassert_no_null(t2d);
    unref(cartesian);
    //Gdiplus::REAL m1[6];
    //Gdiplus::REAL m2[6];
    ctx->graphics->ResetTransform();
    ctx->graphics->TranslateTransform(ctx->offset_x, ctx->offset_y);

    Gdiplus::Matrix mt(
                    (Gdiplus::REAL)t2d->i.x,
                    (Gdiplus::REAL)t2d->i.y,
                    (Gdiplus::REAL)t2d->j.x,
                    (Gdiplus::REAL)t2d->j.y,
                    (Gdiplus::REAL)t2d->p.x,
                    (Gdiplus::REAL)t2d->p.y);

    //Gdiplus::Matrix mt2;
    //ctx->graphics->ResetTransform();
    //ctx->graphics->RotateTransform(45);
    //ctx->graphics->GetTransform(&mt2);
    //mt.GetElements(m1);
    //mt2.GetElements(m2);
    ctx->graphics->MultiplyTransform(&mt);
    dctx_gradient_transform(ctx);

    if (ctx->fpen != NULL)
        ctx->fpen->SetBrush(ctx->current_brush);

    //if (ctx->lbrush != NULL)
    //{
    //    ctx->lbrush->SetTransform(ctx->gradient_matrix);
    //    ctx->lbrush->RotateTransform(ctx->gradient_angle);

    //    //Gdiplus::REAL m[6];
    //    //Gdiplus::Matrix inv;
    //    //ctx->lbrush->GetTransform(&inv);
    //    //mt.GetElements(m);
    //    ////inv.GetElements(m);
    //    //inv.SetElements(m[0], m[1], m[2], m[3], m[4], m[5]);
    //    //inv.Invert();
    //    //ctx->lbrush->SetTransform(&inv);
    //    //ctx->lbrush->RotateTransform(45);
    //}
}

/*---------------------------------------------------------------------------*/

void dctx_gradient_transform(DCtx *ctx)
{
    if (ctx->current_brush == ctx->lbrush)
    {
        Gdiplus::Matrix mt, inv;
        Gdiplus::REAL m[6];
        cassert_no_null(ctx);
        ctx->graphics->GetTransform(&mt);
        mt.GetElements(m);
        inv.SetElements(m[0], m[1], m[2], m[3], m[4], m[5]);
        inv.Invert();
        ctx->lbrush->SetTransform(&inv);
        ctx->lbrush->MultiplyTransform(ctx->gradient_matrix);

        if (ctx->gradient_wrap == Gdiplus::WrapModeClamp)
        {
            Gdiplus::REAL sc = ctx->gradient_scale * 10;
            Gdiplus::REAL len = ctx->gradient_scale + 2 * sc;
            ctx->lbrush->TranslateTransform(ctx->gradient_x, ctx->gradient_y);
            ctx->lbrush->RotateTransform(ctx->gradient_angle);
            ctx->lbrush->TranslateTransform(-sc, 0);
            ctx->lbrush->ScaleTransform(len, 1);
        }
        else
        {
            ctx->lbrush->TranslateTransform(ctx->gradient_x, ctx->gradient_y);
            ctx->lbrush->RotateTransform(ctx->gradient_angle);
            ctx->lbrush->ScaleTransform(ctx->gradient_scale, 1);
        }
    }
}

/*---------------------------------------------------------------------------*/

void draw_clear(DCtx *ctx, const color_t color)
{
    uint8_t r, g, b;
    cassert_no_null(ctx);
    cassert_no_null(ctx->graphics);
    ctx->graphics->Clear(i_color(color));
    color_get_rgb(color, &r, &g, &b);
    ctx->background_color = RGB(r, g, b);
}

/*---------------------------------------------------------------------------*/

void draw_antialias(DCtx *ctx, const bool_t on)
{
    cassert_no_null(ctx);
    ctx->graphics->SetSmoothingMode(on ? Gdiplus::SmoothingModeAntiAlias : Gdiplus::SmoothingModeNone);
    ctx->graphics->SetTextRenderingHint(on ? Gdiplus::TextRenderingHintClearTypeGridFit : Gdiplus::TextRenderingHintSingleBitPerPixelGridFit);
}
