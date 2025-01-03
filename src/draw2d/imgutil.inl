/*
 * NAppGUI Cross-platform C SDK
 * 2015-2025 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: imgutil.inl
 *
 */

/* Image utilities */

#include "draw2d.ixx"

__EXTERN_C

bool_t _imgutil_parse(Stream *stm_in, Stream *stm_out);

uint32_t _imgutil_num_frames(const byte_t *data, const uint32_t size);

Palette *_imgutil_def_palette(const pixformat_t format);

Pixbuf *_imgutil_rgba_to_rgb(const byte_t *data, const uint32_t width, const uint32_t height);

Pixbuf *_imgutil_rgb_to_rgba(const byte_t *data, const uint32_t width, const uint32_t height);

Pixbuf *_imgutil_rgba_to_gray(const byte_t *data, const uint32_t width, const uint32_t height);

Pixbuf *_imgutil_rgb_to_gray(const byte_t *data, const uint32_t width, const uint32_t height);

Buffer *_imgutil_gray_to_rgba(const byte_t *data, const uint32_t width, const uint32_t height);

Buffer *_imgutil_gray_to_rgb(const byte_t *data, const uint32_t width, const uint32_t height);

Pixbuf *_imgutil_indexed_to_gray(const uint32_t width, const uint32_t height, const byte_t *pixdata, const uint32_t stride, const uint32_t ibpp, const color_t *palette);

Pixbuf *_imgutil_indexed_to_rgba(const uint32_t width, const uint32_t height, const byte_t *pixdata, const uint32_t stride, const uint32_t ibpp, const color_t *palette);

Pixbuf *_imgutil_indexed_to_rgb(const uint32_t width, const uint32_t height, const byte_t *pixdata, const uint32_t stride, const uint32_t ibpp, const color_t *palette);

uint32_t _imgutil_effective_palette(const uint32_t *ipalette, const uint32_t isize, uint32_t *opalette, uint8_t *oindex);

Pixbuf *_imgutil_indexed_to_indexed(const uint32_t width, const uint32_t height, const byte_t *pixdata, const uint32_t stride, const uint32_t ibpp, const pixformat_t oformat, const uint8_t *palette_index);

Pixbuf *_imgutil_to_indexed(const uint32_t width, const uint32_t height, const byte_t *pixdata, const uint32_t bytespp, Palette **palette);

__END_C
