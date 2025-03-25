#ifndef PD_STUFF_H
#define PD_STUFF_H
extern char* get_patch_path(const char* patch_id);
extern void* libpd_openfile_by_patch_id(const char* patch_id);
extern char* gen_patch_id();
extern void dsp_on();
extern void dsp_off();
#endif
