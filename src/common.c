#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>

#define PUB

#ifdef DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

typedef uint32_t uint;

#endif COMMON_H

#ifdef DEBUG
PUB void debug_float_array(const float* arr, uint size) {
  debug("[");
  for (int i = 0; i < size; i++) {
    debug("%f, ", arr[i]);
  }
  debug("]\n");
}
#else
#define debug_float_array(a, b)
#endif

/*
void copy_file(char* srcn, char* dstn) {
  FILE* src = fopen(srcn, "r");
  FILE* dst = fopen(dstn, "w");
  int c;
  while ((c = fgetc(src)) != EOF) fputc(c, dst);
  fclose(src);
  fclose(dst);
}
*/


