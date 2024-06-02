#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT whiteball.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/sprite

$GRIT pdemo_bg.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg

$GRIT pdemo_colmap.png -ftB -fh! -gt -gB8 -mRtp -mLf -p!

mv *.img *.map ../nitrofiles/maps
