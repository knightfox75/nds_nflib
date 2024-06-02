#!/bin/sh

GRIT=/opt/blocksds/core/tools/grit/grit

$GRIT bitmap16.jpg -ftB -fh! -gb -gB16

mv *.img ../nitrofiles/bmp
