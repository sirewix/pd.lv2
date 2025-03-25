#!/bin/sh

mkdir -p out
./gen-headers.sh
./gen-plugins.sh > src/plugins.c

gcc \
  -Wno-endif-labels \
  -o out/plugin.so \
  -I include \
  -I . \
  -shared \
  -lpd \
  -fPIC \
  -D DEBUG \
  src/{buffer,common,lv2_stuff,pd_stuff,plugins}.c

rm -rf ~/.lv2/lv2-pd
mkdir -p ~/.lv2/lv2-pd
cp out/plugin.so ~/.lv2/lv2-pd/
cp plugin.ttl ~/.lv2/lv2-pd/
cp manifest.ttl ~/.lv2/lv2-pd/
