#!/bin/bash

exec ./common.sh \
    -Gratio=1 \
    -Grankdir=LR \
    -Nshape=record \
    -Nmargin=0.02,0.02 \
    -Etailport=e \
    -Eheadport=w \
    "$@"
