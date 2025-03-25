#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pd/z_libpd.h"

#include "common.h"

#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
  uint tick_size;
  float* buf; // buffer of size tick_size
  uint cur; // cursor in buf, left of it -- not yet calculated input
            //                right of it -- already calculated output
  float* proc_buf; // dynamic (but never shrinking) buffer for `process` calls
  uint proc_buf_size;
} LeBuffer;

typedef int (*ProcessFunction)(const int, const float*, float*);

#endif BUFFER_H

PUB LeBuffer init_buffer(uint tick_size) {
  LeBuffer buffer;
  buffer.buf = (float*) calloc(tick_size, sizeof(float));
  buffer.tick_size = tick_size;
  buffer.proc_buf_size = 0;
  buffer.proc_buf = NULL;
  buffer.cur = 0;
  return buffer;
}

PUB void free_buffer(const LeBuffer const* buf) {
  free(buf->buf);
  if (buf->proc_buf_size) free(buf->proc_buf);
}

uint min(uint a, uint b) {
  return a <= b ? a : b;
}

#ifdef DEBUG
PUB void debug_buf(const LeBuffer* buf) {
  debug("[");
  for (int i = 0; i < buf->tick_size; i++) {
    if (buf->cur == i) debug(" | ");
    debug("%f, ", buf->buf[i]);
  }
  debug("]\n");
}
#endif

// TODO: Simplify.
// I don't know how it works exactly, it's slightly magical
PUB void poomba(
  LeBuffer* const buf,
  const float* const input,
  float* const output,
  const uint n_samples,
  const ProcessFunction process
) {
  if (n_samples == 0) return;
  const uint tick_size = buf->tick_size;

  const uint in_buf_size = buf->cur;
  const uint out_buf_size = tick_size - buf->cur;

  const uint next_cur = (buf->cur + n_samples) % tick_size;

  const uint from_out_buffer = min(n_samples, out_buf_size);
  const uint into_in_buffer = (in_buf_size + n_samples) % tick_size;

  if (in_buf_size + n_samples >= tick_size) {
    //debug("There's something to process\n");
    const uint ticks_to_process = (in_buf_size + n_samples) / tick_size;
    const uint proc_buf_size = ticks_to_process * tick_size;
    if (proc_buf_size > buf->proc_buf_size) {
      debug("proc_buf_size is of size %i resizing to 2 * %i\n", buf->proc_buf_size, proc_buf_size );
      if (proc_buf_size) free(buf->proc_buf);
      buf->proc_buf_size = proc_buf_size;
      buf->proc_buf = (float*) malloc(2 * proc_buf_size * sizeof(float));
    }

    float* process_buf_in = (float*) buf->proc_buf;
    float* process_buf_out = (float*) &(buf->proc_buf[proc_buf_size]);

    // inbuf -> process
    memcpy(process_buf_in, buf->buf, in_buf_size * sizeof(float));
    // input -> process
    memcpy(
      &(process_buf_in[in_buf_size]),
      input,
      (ticks_to_process * tick_size - in_buf_size) * sizeof(float)
    );
    if (from_out_buffer > 0) {
      memcpy(
        output,
        &(buf->buf[buf->cur]),
        from_out_buffer * sizeof(float)
      );
    }
    // input -> inbuf
    memcpy(
      buf->buf,
      &input[n_samples - next_cur],
      (next_cur) * sizeof(float)
    );

    //debug("Processing %i ticks\n", ticks_to_process);
    process(ticks_to_process, process_buf_in, process_buf_out);
    //debug("Processed %i ticks\n", ticks_to_process);

    // process -> outbuf
    memcpy(
      &buf->buf[next_cur],
      &(process_buf_out[ticks_to_process * tick_size - (tick_size - next_cur)]),
      (tick_size - next_cur) * sizeof(float)
    );
    // process -> output
    memcpy(
      &output[from_out_buffer],
      process_buf_out,
      (ticks_to_process * tick_size - (tick_size - next_cur)) * sizeof(float)
    );

    buf->cur = next_cur;
  } else {
    debug("There's nothing to process\n");
    memcpy(
      output,
      &(buf->buf[buf->cur]),
      from_out_buffer * sizeof(float)
    );
    memcpy(&(buf->buf[buf->cur]), input, n_samples * sizeof(float));
    buf->cur = next_cur;
  }
}
