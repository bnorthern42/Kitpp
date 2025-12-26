!#/usr/bin/bash

echo "compiling examples"

rm -rf build

meson setup build

meson compile -C build
