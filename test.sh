#!/bin/bash

echo "compiling examples and tests"

rm -rf build

meson setup build

meson compile -C build

echo "running tests"

meson test -C build -v
