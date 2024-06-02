#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT img16_c.jpg -ftB -fh! -gb -gB16

mv *.img ../nitrofiles/bmp
