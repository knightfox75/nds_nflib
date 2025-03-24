#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT 8x8.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 16x16.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 32x32.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 64x64.png -ftB -fh! -gTFF00FF -gt -gB8 -m! #All have the same palette so only make the one from this image.
$GRIT 16x8.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 32x8.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 32x16.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 64x32.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 8x16.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 8x32.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 16x32.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!
$GRIT 32x64.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -p!

mv *.pal *.img ../nitrofiles/sprite

$GRIT white_bg.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg
