#!/usr/bin/env bash

KAAPI_HWS_LEVELS=FLAT,NUMA \
KAAPI_EMITSTEAL=hws \
KAAPI_CPUSET=0 \
LD_LIBRARY_PATH=$HOME/install/xkaapi_hws/lib \
./a.out
