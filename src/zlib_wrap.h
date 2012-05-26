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

struct zcmp_handle {
    z_stream *strm;
};

struct zexp_handle {
    z_stream *strm;
};

struct zcom_handle * zcom_create();
int zcom_compress_some();
