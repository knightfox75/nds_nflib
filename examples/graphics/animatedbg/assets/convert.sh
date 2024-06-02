#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT nfl.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
$GRIT water.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img *.map ../nitrofiles/bg
