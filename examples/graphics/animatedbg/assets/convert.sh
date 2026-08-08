#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT nfl.png -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

# Note that tile reduction is disabled: -mR!
$GRIT water.png -ftB -fh! -gTFF00FF -gt -gB8 -m! -mR!

mv *.pal *.img *.map ../nitrofiles/bg
