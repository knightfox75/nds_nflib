#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

# The two backgrounds in the same screen share the palette
$GRIT navygrid.png waves512.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLa -pS -Onavygrid

$GRIT flag512.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLa

# Both backgrounds need to have a palette, even if it's the same one
cp navygrid.pal waves512.pal

mv *.pal *.img *.map ../nitrofiles/bg
