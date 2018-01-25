#!/bin/bash

exec dot -Tsvg \
    -Gsplines=false \
    -Gnodesep=0.05 \
    -Nfontname=Monospace \
    -Nheight=0 \
    -Nwidth=0 \
    "$@"
