#include "lv2/core/lv2.h"
#include "lv2.h"
#define N_CHANNELS 1
#define AMP_URI "https://github.com/sirewix/pd.lv2#mono"
#define plugin_instantiate plugin_instantiate_mono
#define plugin_interleave plugin_interleave_mono
#define plugin_un_interleave plugin_un_interleave_mono
#define plugin_connect_port plugin_connect_port_mono
#define plugin_activate plugin_activate_mono
#define plugin_run plugin_run_mono
#define plugin_deactivate plugin_deactivate_mono
#define plugin_cleanup plugin_cleanup_mono
#define plugin_save plugin_save_mono
#define plugin_restore plugin_restore_mono
#define plugin_extension_data plugin_extension_data_mono
#define PortIndex PortIndex_mono
#define Uris Uris_mono
#define Plugin Plugin_mono
#define plugin_descriptor plugin_descriptor_mono

#include "src/plugin.c"

#undef plugin_instantiate
#undef plugin_interleave
#undef plugin_un_interleave
#undef plugin_connect_port
#undef plugin_activate
#undef plugin_run
#undef plugin_deactivate
#undef plugin_cleanup
#undef plugin_save
#undef plugin_restore
#undef plugin_extension_data
#undef PortIndex
#undef Uris
#undef Plugin
#undef plugin_descriptor
#undef N_CHANNELS
#undef AMP_URI

#define N_CHANNELS 2
#define AMP_URI "https://github.com/sirewix/pd.lv2#stereo"
#define plugin_instantiate plugin_instantiate_stereo
#define plugin_interleave plugin_interleave_stereo
#define plugin_un_interleave plugin_un_interleave_stereo
#define plugin_connect_port plugin_connect_port_stereo
#define plugin_activate plugin_activate_stereo
#define plugin_run plugin_run_stereo
#define plugin_deactivate plugin_deactivate_stereo
#define plugin_cleanup plugin_cleanup_stereo
#define plugin_save plugin_save_stereo
#define plugin_restore plugin_restore_stereo
#define plugin_extension_data plugin_extension_data_stereo
#define PortIndex PortIndex_stereo
#define Uris Uris_stereo
#define Plugin Plugin_stereo
#define plugin_descriptor plugin_descriptor_stereo

#include "src/plugin.c"

#undef plugin_instantiate
#undef plugin_interleave
#undef plugin_un_interleave
#undef plugin_connect_port
#undef plugin_activate
#undef plugin_run
#undef plugin_deactivate
#undef plugin_cleanup
#undef plugin_save
#undef plugin_restore
#undef plugin_extension_data
#undef PortIndex
#undef Uris
#undef Plugin
#undef plugin_descriptor
#undef N_CHANNELS
#undef AMP_URI


static const LV2_Descriptor descriptors[] = {
  plugin_descriptor_mono,
  plugin_descriptor_stereo,
NULL };
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) { return &descriptors[index]; }
