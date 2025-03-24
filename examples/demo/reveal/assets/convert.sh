#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT ball.png -ftB -fh! -gTFF00FF -gt -gB8 -m!

mv *.pal *.img ../nitrofiles/spr

# The two backgrounds that share the palette have been edited to use 128 colors
# each, so that the final combined palette is 256 colors in size.

$GRIT img8b_1.png img8b_2.png -ftB -fh! -gTFF00FF -gb -gB8 -pS -Oimg8b_1.pal

$GRIT nfl.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

# This is required or the library will say that the second background doesn't
# have a palette.

cp img8b_1.pal img8b_2.pal

mv *.pal *.img *.map ../nitrofiles/bg
