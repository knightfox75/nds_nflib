#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT blueball.png -ftB -fh! -gTFF00FF -gb -gB8 -m!

mv *.pal *.img ../nitrofiles/sprite

$GRIT nature.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg
