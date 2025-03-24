#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

for file in *.png; do
    $GRIT "$file" -ftB -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
done

mv *.pal *.img *.map ../nitrofiles/bg
