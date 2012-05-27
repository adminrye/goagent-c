/*
 * zlib_wrap.c
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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "zlib.h"
#include "buffer.h"

#define CHUNK 16 * 1024

z_stream * zcmp_open() {
    z_stream *strm; 
    int ret;

    strm = calloc(1, sizeof *strm);
    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;
    ret = deflateInit(strm, -1);
    if (ret != Z_OK) {
        free(strm);
        return NULL;
    }
    return strm;
}

static void _zcmp_compress(z_stream *strm, char *in, uint32_t len, struct buffer *buffer, int close) {
    int ret;

    strm->avail_in = len;
    strm->next_in = in;
    do {
        buffer_prepare_space(buffer);
        strm->avail_out = NODE_SIZE - buffer->last->end;
        strm->next_out = buffer->last->buffer + buffer->last->end;
        /* no bad return value */
        ret = deflate(strm, close ? Z_FINISH : Z_NO_FLUSH);
        /* state not clobbered */
        assert(ret != Z_STREAM_ERROR);
        size_t newend = NODE_SIZE - strm->avail_out;
        buffer->len += newend - buffer->last->end;
        buffer->last->end = newend;
    } while (strm->avail_out == 0);
}

void zcmp_compress(z_stream *strm, char *in, uint32_t len, struct buffer *buffer) {
    _zcmp_compress(strm, in, len, buffer, 0);
}

void zcmp_close(z_stream *strm, char *in, uint32_t len, struct buffer *buffer) {
    _zcmp_compress(strm, in, len, buffer, 1);
    deflateEnd(strm);
    free(strm);
}

z_stream * zexp_open() {
    int ret;
    z_stream *strm; 
    strm = calloc(1, sizeof *strm);

    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;
    strm->avail_in = 0;
    strm->next_in = Z_NULL;

    ret = inflateInit(strm);
    if (ret != Z_OK) {
        free(strm);
        return NULL;
    }
    return strm;
}

int zexp_expand(z_stream *strm, char *in, uint32_t len, struct buffer *buffer) {
    int ret;

    strm->avail_in = len;
    strm->next_in = in;
    do {
        buffer_prepare_space(buffer);
        strm->avail_out = NODE_SIZE - buffer->last->end;
        strm->next_out = buffer->last->buffer + buffer->last->end;
        ret = inflate(strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(strm);
                return ret;
        }
        size_t newend = NODE_SIZE - strm->avail_out;
        buffer->len += newend - buffer->last->end;
        buffer->last->end = newend;
    } while (strm->avail_out == 0);
    return ret;
}

void zexp_close(z_stream *strm) {
    inflateEnd(strm);
    free(strm);
}
