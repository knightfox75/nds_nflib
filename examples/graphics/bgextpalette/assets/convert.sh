#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT blue.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
# Only generate palette for the other two
$GRIT red.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs -m! -g!
$GRIT green.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs -m! -g!

mv *.pal *.img *.map ../nitrofiles/bg

$GRIT default.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/fnt
