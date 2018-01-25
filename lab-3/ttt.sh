#!/bin/bash

RATIO=0.618 # ~= 1/phi

exec ./common.sh \
    -Gratio=$RATIO \
    -Grankdir=TB \
    -Nshape=rect \
    -Nmargin=0,0 \
    -Etailport=s \
    -Eheadport=n \
    "$@"
