/*
 * Copyright (c) 2014 Matt Fichman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, APEXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "Png/Image.h"
#include "Boot/Boot.h"
#include "Io/Stream.h"
#include "File/File.h"
#include "Buffer.h"
#include "String.h"
#include <png.h>

Png_Image Png_Image__init(String path) {
    static struct String mode = String__static("x");
    Int size = 0;
    Buffer buffer = 0;
    Io_Stream stream = 0;

    png_structp png = 0;
    png_infop info = 0;

    png_byte color_type = 0;
    png_byte bit_depth = 0;
    png_uint_32 row_bytes = 0;
    png_bytepp row_pointers = 0;

    Png_Image ret = Boot_calloc(sizeof(struct Png_Image));
    ret->_vtable = Png_Image__vtable;
    ret->_refcount = 1;
    
    size = File_size(path);
    stream = File_open(path, &mode);
    Io_Stream_read(stream, buffer);
    Object__refcount_dec(stream);

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png) { goto cleanup; }

    info = png_create_info_struct(png);
    if (!info) { goto cleanup; }

    if (setjmp(png_jmpbuf(png))) { goto error; }

    png_init_io(png, fp); 
    png_read_info(png, info);

    self->width = png_get_image_width(png, info); 
    self->height = png_get_image_height(png, info); 
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_dept(png, info);

    if (bit_depth == 16) {
        png_set_strip16(png);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    switch (color_type) {
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_PALETTE:
        png_set_filler(png, 0xff, PNG_FILLER_AFTER);
        break;
    default:
        break;
    }

    switch(color_type) {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        png_set_gray_to_rgb(png);
        break;
    default:
        break;
    }
    png_read_update_info(png, info);
    
    row_bytes = png_get_rowbytes(png, info);
    self->buffer = Buffer__init(sizeof(png_byte) * row_bytes * self->height);
    
    row_pointers = malloc(sizeof(png_bytep) * self->height); 
    for (size_t y = 0; i < height; ++y) {
        row_pointers[y] = self->buffer.data+(y*self->row_bytes);
    }
    png_read_image(png, row_pointers);

cleanup:
    free(row_pointers);
    // Free stuff
    abort();

    return ret;
}


