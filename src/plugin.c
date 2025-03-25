#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lv2.h"
#include "lv2/atom/atom.h"
#include "lv2/atom/util.h"
#include "lv2/midi/midi.h"
#include "lv2/state/state.h"
#include "lv2/urid/urid.h"
#include "pd/z_libpd.h"

#include "common.h"
#include "buffer.h"
#include "lv2_stuff.h"
#include "default_patch.h"
#include "pd_stuff.h"

#ifndef N_CHANNELS
#define N_CHANNELS 2
#define AMP_URI "https://github.com/sirewix/pd.lv2#stereo"
#endif

// TODO: LV2_UI__showInterface

#define PORT_INDEXES 1
// typedef enum { PORT_INDEXES = 0 } PortIndex;

typedef struct {
  LV2_URID atom_String;
  LV2_URID midi_MidiEvent;
  LV2_URID patch_id;
} Uris;

typedef struct {
  const LV2_Atom_Sequence* control;
  const float*  input[N_CHANNELS];
  float*        output[N_CHANNELS];
  void*         patch_fd;
  t_pdinstance* pd_instance;
  LeBuffer      buf;
  float*        ibuf;
  uint          ibuf_size;
  Uris          uris;
  char*         patch_id;
  //bool          active;
  bool          saved;
} Plugin;

#define NS_MY "https://github.com/sirewix/pd.lv2#schema-"

PUB static LV2_Handle plugin_instantiate(
  const LV2_Descriptor*     descriptor,
  double                    rate,
  const char*               bundle_path,
  const LV2_Feature* const* features
) {
  printf("PD PLUGIN BY WIX %f\n", rate);
  srand(time(NULL));
  // list_features(features);
  libpd_init(); // if (libpd_init()) { printf("libpd init failed\n"); return NULL; }
  Plugin* plugin = (Plugin*)calloc(1, sizeof(Plugin));
  plugin->pd_instance = libpd_new_instance();
  if (plugin->pd_instance == NULL) { printf("libpd_new_instance failed\n"); return NULL; }
  libpd_set_instance(plugin->pd_instance);

  LV2_URID_Map* map_uri = find_feature(features, LV2_URID__map);
  //plugin->uris.atom_Path = map_uri->map(map_uri->handle, LV2_ATOM__Path);
  plugin->uris.atom_String = map_uri->map(map_uri->handle, LV2_ATOM__String);
  plugin->uris.midi_MidiEvent = map_uri->map(map_uri->handle, LV2_MIDI__MidiEvent);
  //plugin->uris.patch_path = map_uri->map(map_uri->handle, NS_MY "patch");
  plugin->uris.patch_id = map_uri->map(map_uri->handle, NS_MY "patch");
  // plugin->state.path = strdup("patch.pd");

  if (libpd_init_audio(N_CHANNELS, N_CHANNELS, (uint)rate)) { printf("libpd audio init failed\n"); return NULL; }

  plugin->buf = init_buffer(libpd_blocksize() * N_CHANNELS);
  return (LV2_Handle)plugin;
}

PUB void plugin_interleave(Plugin* plugin, float* interleaved_output, uint32_t n_spluginles) {
  for (uint s = 0; s < n_spluginles; s++) {
    for (uint c = 0; c < N_CHANNELS; c++) {
      interleaved_output[s * N_CHANNELS + c] = plugin->input[c][s];
    }
  }
}

PUB void plugin_un_interleave(Plugin* plugin, float* interleaved_input, uint32_t n_spluginles) {
  for (uint s = 0; s < n_spluginles; s++) {
    for (uint c = 0; c < N_CHANNELS; c++) {
      plugin->output[c][s] = interleaved_input[s * N_CHANNELS + c];
    }
  }
}

PUB static void plugin_connect_port(LV2_Handle instance, uint32_t port, void* data) {
  Plugin* plugin = (Plugin*)instance;
  const uint IN_CHANNELS_START = PORT_INDEXES;
  const uint OUT_CHANNELS_START = PORT_INDEXES + N_CHANNELS;
  if (port == 0) {
    plugin->control = (const LV2_Atom_Sequence*)data;
  } else if (port >= OUT_CHANNELS_START && port - OUT_CHANNELS_START < N_CHANNELS) {
    plugin->output[port - OUT_CHANNELS_START] = (float*) data;
  } else if (port >= IN_CHANNELS_START && port - IN_CHANNELS_START < N_CHANNELS) {
    plugin->input[port - IN_CHANNELS_START] = (const float*) data;
  }
}

PUB static void plugin_activate(LV2_Handle instance) {
  Plugin* plugin = (Plugin*)instance;
  debug("ACTIVATE CALLED\n");
  libpd_set_instance(plugin->pd_instance);
  // if (plugin->patch_fd == NULL) { debug("cannot libpd_openfile\n"); }
  dsp_on();
  libpd_start_gui("/usr/lib/pd");
}


PUB static void plugin_run(LV2_Handle instance, uint32_t n_spluginles) {
  Plugin* plugin = (Plugin*)instance;
  libpd_set_instance(plugin->pd_instance);

  // debug("got %i samples\n", n_spluginles);
  int port = libpd_getdollarzero(plugin->patch_fd);

  LV2_ATOM_SEQUENCE_FOREACH (plugin->control, ev) {
    if (ev->body.type == plugin->uris.midi_MidiEvent) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      uint8_t type = msg[0];
      uint8_t data1 = msg[1];
      uint8_t data2 = msg[2];
      int channel = type & 0x0FU;
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_NOTE_ON:
        debug("note on %i %i %i\n", type & 0x0FU, data1, data2);
        if (libpd_noteon(channel, data1, data2)) debug("libpd_noteon failed\n");
        break;

      case LV2_MIDI_MSG_NOTE_OFF:
        debug("note off %i %i %i\n", type & 0x0FU, data1, data2);
        if (libpd_noteon(channel, data1, 0)) debug("libpd_noteon failed\n");
        break;

      default:
        if (lv2_midi_is_system_message(msg)) {
          if (libpd_controlchange(channel, data1, data2)) debug("libpd_controlchange failed\n");
          break;
        }
        debug("unknown midi msg %x %x %x\n", type & 0xFF, data1 & 0xFF, data2 & 0xFF);
        break;
      }
    }
  }

  ///

  int poll_count = 100;
  while (libpd_poll_gui() && poll_count-- > 0) {};

  if (plugin->ibuf_size < n_spluginles) {
    plugin->ibuf = malloc(2 * n_spluginles * N_CHANNELS * sizeof(float));
    plugin->ibuf_size = n_spluginles;
  }

  float* in_buf = plugin->ibuf;
  float* out_buf = &plugin->ibuf[n_spluginles * N_CHANNELS];

  plugin_interleave(plugin, in_buf, n_spluginles);
  poomba(&plugin->buf, in_buf, out_buf, n_spluginles * N_CHANNELS, libpd_process_float);
  plugin_un_interleave(plugin, out_buf, n_spluginles);
}

PUB static void plugin_deactivate(LV2_Handle instance) {
  const Plugin* plugin = (const Plugin*)instance;
  libpd_set_instance(plugin->pd_instance);
  // if (libpd_getdollarzero(plugin->patch_fd)) libpd_closefile(plugin->patch_fd);
  dsp_off();
  libpd_stop_gui();
}

PUB static void plugin_cleanup(LV2_Handle instance) {
  const Plugin* plugin = (const Plugin*)instance;
  debug("CLEANUP\n");
  libpd_set_instance(plugin->pd_instance);
  if (libpd_getdollarzero(plugin->patch_fd)) libpd_closefile(plugin->patch_fd);
  debug("Stopping pd gui\n");
  libpd_stop_gui();
  debug("Freeing pd instance\n");
  libpd_free_instance(plugin->pd_instance);

  debug("Freeing pd buffer\n");
  free_buffer(&plugin->buf);
  if(plugin->ibuf_size) {
    debug("Freeing pd interleave buffer\n");
    free(plugin->ibuf);
  }
  debug("Freeing pd plugin instance\n");
  free(instance);
}

PUB LV2_State_Status plugin_save(
  LV2_Handle                instance,
  LV2_State_Store_Function  store,
  LV2_State_Handle          handle,
  uint32_t                  flags,
  const LV2_Feature* const* features
) {
  Plugin* plugin = (Plugin*)instance;
  debug("SAVE IS CALLED\n");
  libpd_set_instance(plugin->pd_instance);

  // if (!plugin->state.state_path) {
  /*
    LV2_State_Make_Path* make_path = find_feature(features, LV2_STATE__makePath);
    if (make_path == NULL) { printf("makePath feature is not found\n"); }
    char* make_path_res = make_path->path(make_path->handle, "patch.pd");
    debug("make_path_res: %s\n", make_path_res );

    copy_file(plugin->state.path, make_path_res);
    //plugin->state.state_path = true;
    plugin->state.path = make_path_res;

    if (libpd_getdollarzero(plugin->patch_fd)) libpd_closefile(plugin->patch_fd);
    plugin->patch_fd = libpd_openfile_one_path(make_path_res);

    if (plugin->patch_fd == NULL) { debug("cannot libpd_openfile\n"); return LV2_STATE_ERR_UNKNOWN; }
    */

  if (plugin->patch_id) {
    plugin->saved = true;
    debug("saving patch_id %s\n", plugin->patch_id);
    store(
      handle,
      plugin->uris.patch_id,
      plugin->patch_id,
      strlen(plugin->patch_id) + 1,
      plugin->uris.atom_String,
      LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE
    );
  }

  return LV2_STATE_SUCCESS;
}

PUB LV2_State_Status plugin_restore(
  LV2_Handle                  instance,
  LV2_State_Retrieve_Function retrieve,
  LV2_State_Handle            handle,
  uint32_t                    flags,
  const LV2_Feature* const*   features
) {
  Plugin* plugin = (Plugin*)instance;
  debug("RESTORE IS CALLED\n");
  libpd_set_instance(plugin->pd_instance);

  const char* patch_id = retrieve(handle, plugin->uris.patch_id, NULL, NULL, NULL);
  if (patch_id == NULL) {
    debug("No patch id\n");
    patch_id = gen_patch_id();
    char* patch_path = get_patch_path(patch_id);
    debug("creating new patch at %s\n", patch_path);
    FILE* tf = fopen(patch_path, "w");
    fputs(DEFAULT_PATCH, tf);
    free(patch_path);
    fclose(tf);
  } else {
    if (plugin->patch_id && !plugin->saved) {
      char* patch_path = get_patch_path(plugin->patch_id);
      remove(patch_path);
      free(patch_path);

      if (libpd_getdollarzero(plugin->patch_fd)) libpd_closefile(plugin->patch_fd);
    }
    plugin->saved = true;
  }

  plugin->patch_id = strdup(patch_id);
  plugin->patch_fd = libpd_openfile_by_patch_id(plugin->patch_id);
  if (plugin->patch_fd == NULL) { debug("cannot libpd_openfile\n"); return LV2_STATE_ERR_UNKNOWN; }

  return LV2_STATE_SUCCESS;
}

PUB static const void* plugin_extension_data(const char* uri) {
  static const LV2_State_Interface state_iface = { .restore = plugin_restore, .save = plugin_save };
  if (!strcmp(uri, LV2_STATE__interface)) { return &state_iface; }
  return NULL;
}

PUB static const LV2_Descriptor plugin_descriptor = {
  AMP_URI,
  plugin_instantiate,
  plugin_connect_port,
  plugin_activate,
  plugin_run,
  plugin_deactivate,
  plugin_cleanup,
  plugin_extension_data
};

// LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) { return index == 0 ? &plugin_descriptor : NULL; }
