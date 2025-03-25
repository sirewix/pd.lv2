#!/bin/sh

mkdir -p include

gen_header() {
  f=$1
  mod="$(basename $f .c)";
  hf="$mod.h"
  hg="$(echo $mod | tr '[a-z]' '[A-Z]')_H"

  # awk "/PUB .* {/" $f
  echo "#ifndef $hg"
  echo "#define $hg"
  from="#define $hg"
  to="#endif $hg"
  sed -n "/$from/,\${p;/$to/q}" $f | sed "/$from/d; /$to/d"
  awk '/PUB .*\(/,/\)/' $f | sed 's/ {$/;/; s/^PUB/extern/'
  echo "#endif"
}

for f in $(ls src/*.c); do
  hf="$(basename $f .c).h";
  gen_header "$f" > include/$hf
done

for f in $(ls include/*.h); do
  loc=$(wc -l $f | cut -f1 -d' ')
  [ $loc = '3' ] && rm $f
done

exit 0
