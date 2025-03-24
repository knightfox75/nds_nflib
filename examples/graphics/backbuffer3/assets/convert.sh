#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT bitmap16.png -ftB -fh! -gb -gB16
$GRIT img16_a.png -ftB -fh! -gb -gB16
$GRIT img16_b.png -ftB -fh! -gb -gB16

mv *.img ../nitrofiles/bmp
