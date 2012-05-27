/*
 * zlib_wrap.h
 *
 * Copyright xubin
 *
 * Author : xubin <nybux.tsui@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "zlib.h"
#include "buffer.h"

z_stream * zcmp_open();
void zcmp_compress(z_stream *strm, char *in, uint32_t len, struct buffer *buffer);
void zcmp_close(z_stream *strm, char *in, uint32_t len, struct buffer *buffer);

z_stream * zexp_open();
void zexp_expand(z_stream *strm, char *in, uint32_t len, struct buffer *buffer);
void zexp_close(z_stream *strm);

