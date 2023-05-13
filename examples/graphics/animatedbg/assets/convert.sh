#!/bin/sh

grit nfl.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
grit water.png -ftb -fh! -gTFF00FF -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ../nitrofiles/bg
