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

extern LeBuffer init_buffer(uint tick_size);
extern void free_buffer(const LeBuffer const* buf);
extern void debug_buf(const LeBuffer* buf);
extern void poomba(
  LeBuffer* const buf,
  const float* const input,
  float* const output,
  const uint n_samples,
  const ProcessFunction process
);
#endif
