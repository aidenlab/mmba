#!/bin/sh

set -x 

LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libtcmalloc.so.4 ./tmul.exe chr1_1K.h5 10000000 4
