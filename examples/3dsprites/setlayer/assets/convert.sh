#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT numbers.png -ftB -fh! -gTFF00FF -gb -gB8 -m!

mv *.pal *.img ../nitrofiles/sprite

$GRIT nfl.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
$GRIT bg1.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
$GRIT bg2.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
$GRIT bg3.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

mv *.pal *.img *.map ../nitrofiles/bg
