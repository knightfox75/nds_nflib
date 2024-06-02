#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT layer3.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg

$GRIT default.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/fnt
