#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT ball.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/sprite

$GRIT img8b_1.png -ftB -fh! -gTFF00FF -gb -gB8
$GRIT img8b_2.png -ftB -fh! -gTFF00FF -gb -gB8

mv *.pal *.img ../nitrofiles/bmp

$GRIT bg0.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg

$GRIT font16.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/fnt
