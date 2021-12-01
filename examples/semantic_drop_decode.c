/*
 *  Copyright (c) 2012 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/*
 * This is an example demonstrating how to implement a multi-layer
 * VP9 encoding scheme based on spatial scalability for video applications
 * that benefit from a scalable bitstream.
 */

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../args.h"
#include "../tools_common.h"
#include "../video_writer.h"

#include "vpx/vpx_decoder.h"
#include "../video_reader.h"
#include "./vpx_config.h"

#include "../vpx_ports/vpx_timer.h"
#include "./svc_context.h"
#include "vpx/vp8cx.h"
#include "vpx/vpx_encoder.h"
#include "../vpxstats.h"
#include "vp9/encoder/vp9_encoder.h"
#include "./y4minput.h"

static const char *exec_name;

void usage_exit(void) {
  fprintf(stderr, "Usage: %s <infile> <outfile>\n", exec_name);
  exit(EXIT_FAILURE);
}

static vpx_codec_err_t parse_superframe_index(const uint8_t *data,
                                              size_t data_sz, uint64_t sizes[8],
                                              int *count) {
  // A chunk ending with a byte matching 0xc0 is an invalid chunk unless
  // it is a super frame index. If the last byte of real video compression
  // data is 0xc0 the encoder must add a 0 byte. If we have the marker but
  // not the associated matching marker byte at the front of the index we have
  // an invalid bitstream and need to return an error.

  uint8_t marker;

  marker = *(data + data_sz - 1);
  *count = 0;

  if ((marker & 0xe0) == 0xc0) {
    const uint32_t frames = (marker & 0x7) + 1;
    const uint32_t mag = ((marker >> 3) & 0x3) + 1;
    const size_t index_sz = 2 + mag * frames;

    // This chunk is marked as having a superframe index but doesn't have
    // enough data for it, thus it's an invalid superframe index.
    if (data_sz < index_sz) return VPX_CODEC_CORRUPT_FRAME;

    {
      const uint8_t marker2 = *(data + data_sz - index_sz);

      // This chunk is marked as having a superframe index but doesn't have
      // the matching marker byte at the front of the index therefore it's an
      // invalid chunk.
      if (marker != marker2) return VPX_CODEC_CORRUPT_FRAME;
    }

    {
      // Found a valid superframe index.
      uint32_t i, j;
      const uint8_t *x = &data[data_sz - index_sz + 1];

      for (i = 0; i < frames; ++i) {
        uint32_t this_sz = 0;

        for (j = 0; j < mag; ++j) this_sz |= (*x++) << (j * 8);
        sizes[i] = this_sz;
      }
      *count = frames;
    }
  }
  return VPX_CODEC_OK;
}

int main(int argc, const char **argv) {
  int n_bitrates = 0;
  int bitrate_to_id[500];
  VpxVideoReader *readers[100];
  int bitrates[100];
  int i, frame_cnt, bitrate, layer;

  if ( argc != 4 ) {
    fprintf(stderr, "./semantic_drop_decode [prefix] [trace] [output]\n");
    return -1;
  }
  FILE* fptr = fopen(argv[2], "r");
  if (!fptr) {
    fprintf(stderr, "trace file not exists");
    return -1;
  }
  fscanf(fptr, "%d", &n_bitrates);
  for (i = 0; i < n_bitrates; ++i) {
    int bitrate = 0;
    fscanf(fptr, "%d", &bitrate);
    bitrate_to_id[(int)bitrate/100] = i;
    bitrates[i] = bitrate;
  }
  fscanf(fptr, "\n");
  for (i = 0; i < n_bitrates; ++i) {
    char fname[100];
    memset(fname, 0, 100);
    sprintf(fname, "%s_%d", argv[1], bitrates[i]);
    readers[i] = vpx_video_reader_open(fname);
    //fprintf(stderr, "%s\n", fname);
    if (!readers[i]) {
      fprintf(stderr, "failed to open %s\n", fname);
    }
  }

  VpxVideoWriter *writer = NULL;
  VpxVideoInfo *info = NULL;
  info = vpx_video_reader_get_info(readers[2]);
  printf("w: %d h: %d num: %d denom: %d", \
      info->frame_width, info->frame_height, info->time_base.numerator, info->time_base.denominator);
  info->time_base.numerator = 1;
  info->time_base.denominator = 30;

  writer = vpx_video_writer_open(argv[3], kContainerIVF, info);
  while (fscanf(fptr, "%d %d %d\n", &layer, &frame_cnt, &bitrate) != EOF) {
    //fprintf(stderr, "%d %d %d", layer, frame_cnt, bitrate);
    int vid = bitrate_to_id[(int)bitrate/100];
    size_t frame_size = 0;
    for (i = 0; i < n_bitrates; ++i) {
      vpx_video_reader_read_frame(readers[i]);
    }
    const unsigned char *frame = vpx_video_reader_get_frame(readers[vid], &frame_size);
    uint64_t sizes_parsed[8];
    int count, write_size = 0;
    parse_superframe_index( frame, frame_size, sizes_parsed, &count );
    for (i = 0; i < layer; ++i) write_size += sizes_parsed[i];
    printf("frame: %d read_size: %lu write_size: %llu\n", frame_cnt, frame_size, write_size );
    //printf("frame: %d read_size: %lu %lu %lu %lu\n", 
    //  frame_cnt, frame_size, sizes_parsed[0], sizes_parsed[1], sizes_parsed[2]);
    vpx_video_writer_write_frame(writer, frame, write_size, frame_cnt);
  }

  vpx_video_writer_close(writer);
  for (i = 0; i < n_bitrates; ++i) {
    vpx_video_reader_close(readers[i]);
  }

  // int frame_cnt = 0;
  // FILE *outfile = NULL;
  // VpxVideoReader *reader = NULL;
  // VpxVideoWriter *writer = NULL;
  // VpxVideoInfo *info = NULL;

  // //reader = vpx_video_reader_open(argv[1]);
  // info = vpx_video_reader_get_info(readers[0]);

  // printf("w: %d h: %d num: %d denom: %d", \
  //   info->frame_width, info->frame_height, info->time_base.numerator, info->time_base.denominator);
  // info->time_base.numerator = 1;
  // info->time_base.denominator = 30;

  // writer = vpx_video_writer_open(argv[2], kContainerIVF, info);
  // if (!writer || !reader) die("Failed to open %s or %s\n", argv[1], argv[2] );

  // while (vpx_video_reader_read_frame(reader)) {
  //   size_t frame_size = 0;
  //   const unsigned char *frame = vpx_video_reader_get_frame(reader, &frame_size);
  //   uint64_t sizes_parsed[8];
  //   int count;
  //   parse_superframe_index( frame, frame_size, sizes_parsed, &count );

  //   //printf("frame: %d read_size: %lu write_size: %llu\n", frame_cnt, frame_size, sizes_parsed[0] );

  //   vpx_video_writer_write_frame(writer, frame, sizes_parsed[0], frame_cnt);

  //   frame_cnt += 1;
  // }

  // vpx_video_reader_close(reader);
  // vpx_video_writer_close(writer);

  // fclose(outfile);
}
