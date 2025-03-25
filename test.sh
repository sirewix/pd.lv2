#!/bin/sh
set -eu
set -x

./gen-headers.sh

gcc \
  -Wno-endif-labels \
  -o out/test \
  -I include \
  -l pd \
  -D DEBUG='' \
  src/{test,buffer,common,pd_stuff}.c

./out/test
