#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "pd/z_libpd.h"
#include "common.h"

// yes, what are going to do about it?
char* split_path(char* path) {
  int last_separator = -1;
  int i = 0;
  while (path[i]) {
    if (path[i] == '/') last_separator = i;
    i++;
  }
  if (last_separator == -1) {
    return NULL;
  } else {
    path[last_separator] = 0;
    return &path[last_separator + 1];
  }
}

void un_split_path(char* base_path) {
  base_path[strlen(base_path)] = '/';
}

void* libpd_openfile_one_path(char* path) {
  char* base_dir = path;
  char* patch_file = split_path(path);

  void* fd = libpd_openfile(patch_file, base_dir);
  un_split_path(base_dir);

  return fd;
}

char* get_homedir() {
#ifdef _WIN32
  char* homedrive = getenv("HOMEDRIVE");
  char* homepath = getenv("HOMEPATH");
  uint path_len = strlen(homedrive) + strlen(homepath);
  char* homedir = malloc(path_len + 1);
  sprintf(homedir, "%s%s", homedrive, homepath);
#else
  char* homedir = strdup(getenv("HOME"));
#endif
  return homedir;
}

PUB char* get_patch_path(const char* patch_id) {
  char* homedir = get_homedir();
  char* patch_path = (char*)malloc(strlen(homedir) + strlen(patch_id) + 11 + 1);
  sprintf(patch_path, "%s/lv2-pd/%s.pd", homedir, patch_id);
  free(homedir);
  return patch_path;
}

PUB void* libpd_openfile_by_patch_id(const char* patch_id) {
  char* patch_path = get_patch_path(patch_id);
  void* h = libpd_openfile_one_path(patch_path);
  free(patch_path);
  return h;
}

#define PATCH_ID_LEN 8

PUB char* gen_patch_id() {
  char id[PATCH_ID_LEN * 2 + 1];
  id[PATCH_ID_LEN * 2] = 0;
  for (int i = 0; i < PATCH_ID_LEN; i++) {
    unsigned char n = (unsigned char) rand();
    unsigned char n1 = (n & 0xF0U) >> 4;
    unsigned char n2 = n & 0x0FU;
    id[i*2]   = n1 <= 9 ? n1 + '0' : n1 - 10 + 'a';
    id[i*2+1] = n2 <= 9 ? n2 + '0' : n2 - 10 + 'a';
  }
  return strdup(&id[0]);
}

PUB void dsp_on() {
  libpd_start_message(1);
  libpd_add_float(1.0f);
  libpd_finish_message("pd", "dsp");
}

PUB void dsp_off() {
  libpd_start_message(1);
  libpd_add_float(0.0f);
  libpd_finish_message("pd", "dsp");
}
