#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT pointer.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/sprite

$GRIT colmap.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
$GRIT layer3.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg

$GRIT cmap.png -ftB -fh! -g! -gB8 -mRt -mLf -p!

mv *.map ../nitrofiles/maps

$GRIT default.png -ftB -fh! -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/fnt
