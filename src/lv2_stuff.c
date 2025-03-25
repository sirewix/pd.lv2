#include <string.h>
#include "lv2.h"
#include "common.h"

PUB void* find_feature(const LV2_Feature* const* features, char* feat_uri) {
  for (LV2_Feature* const* feature = (LV2_Feature* const*) features; *feature != NULL; feature++) {
    if (!strcmp(feat_uri, (*feature)->URI)) {
      debug("found feature %s\n", (*feature)->URI);
      return (*feature)->data;
    }
  }
  return NULL;
}

PUB void list_features(const LV2_Feature* const* features) {
  for (LV2_Feature* const* feature = (LV2_Feature* const*) features; *feature != NULL; feature++) {
    debug("available feature %s\n", (*feature)->URI);
  }
}

