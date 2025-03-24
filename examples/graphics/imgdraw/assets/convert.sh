#!/bin/bash

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

$GRIT bitmap16.png -ftB -fh! -gb -gB16
$GRIT blueball.png -ftB -fh! -gb -gB16

mv *.img ../nitrofiles/bmp
