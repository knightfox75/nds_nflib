#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT nfl.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg
