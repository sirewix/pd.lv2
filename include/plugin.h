#ifndef PLUGIN_H
#define PLUGIN_H
extern static LV2_Handle plugin_instantiate(
  const LV2_Descriptor*     descriptor,
  double                    rate,
  const char*               bundle_path,
  const LV2_Feature* const* features
);
extern void plugin_interleave(Plugin* plugin, float* interleaved_output, uint32_t n_spluginles);
extern void plugin_un_interleave(Plugin* plugin, float* interleaved_input, uint32_t n_spluginles);
extern static void plugin_connect_port(LV2_Handle instance, uint32_t port, void* data);
extern static void plugin_activate(LV2_Handle instance);
extern static void plugin_run(LV2_Handle instance, uint32_t n_spluginles);
extern static void plugin_deactivate(LV2_Handle instance);
extern static void plugin_cleanup(LV2_Handle instance);
extern LV2_State_Status plugin_save(
  LV2_Handle                instance,
  LV2_State_Store_Function  store,
  LV2_State_Handle          handle,
  uint32_t                  flags,
  const LV2_Feature* const* features
);
extern LV2_State_Status plugin_restore(
  LV2_Handle                  instance,
  LV2_State_Retrieve_Function retrieve,
  LV2_State_Handle            handle,
  uint32_t                    flags,
  const LV2_Feature* const*   features
);
extern static const void* plugin_extension_data(const char* uri);
#endif
