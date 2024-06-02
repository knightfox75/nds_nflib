#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT bitmap16.png -ftB -fh! -gb -gB16

mv *.img ../nitrofiles/bmp
