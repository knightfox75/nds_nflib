#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT img16_c.jpg -ftB -fh! -gb -gB16

mv *.img ../nitrofiles/bmp
