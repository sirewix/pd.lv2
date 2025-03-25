#!/bin/sh

AMP_URI='https://github.com/sirewix/pd.lv2'
channels='1 2'

get_symbols() {
  ( grep -o '[a-zA-Z_]\+(' include/plugin.h;
    # awk '/typedef .*\(/,/\)/' $f | sed 's/ {$/;/; s/^PUB/extern/'
    # grep -o '} [a-zA-Z_]\+;' include/amp.h
    grep -o '} [a-zA-Z_]\+;' src/plugin.c
  ) | sed 's/} //; s/\;//; s/(//'
}

symbols="$(get_symbols) plugin_descriptor"

v_name() { ch=$1
  case $ch in
    1) echo mono ;;
    2) echo stereo ;;
    *) echo "ch${ch}" ;;
  esac
}

gen_include_entry() { ch=$1
  echo "#define N_CHANNELS $ch"
  echo "#define AMP_URI \"${AMP_URI}#$(v_name $ch)\""

  for symbol in $symbols; do
    echo "#define $symbol ${symbol}_$(v_name $ch)"
  done

  echo
  echo '#include "src/plugin.c"'
  echo

  for symbol in $symbols; do
    echo "#undef $symbol"
  done

  echo '#undef N_CHANNELS'
  echo '#undef AMP_URI'
  echo
}

gen_plugins_c() {
  echo '#include "lv2/core/lv2.h"'
  echo '#include "lv2.h"'
  for ch in $channels; do
    gen_include_entry $ch
  done
  echo

  echo 'static const LV2_Descriptor descriptors[] = {'
  for ch in $channels; do
    echo "  plugin_descriptor_$(v_name $ch),"
  done
  echo 'NULL };'

  echo 'LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) { return &descriptors[index]; }'
}

gen_plugins_c
