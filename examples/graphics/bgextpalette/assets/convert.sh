#!/bin/sh

grit blue.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
# Only generate palette for the other two
grit red.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs -m! -g!
grit green.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs -m! -g!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ../nitrofiles/bg

grit default.png -ftb -fh! -gTFF00FF -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

for file in *.img; do
    mv -- "$file" "${file%.img}".fnt
done

mv *.pal *.fnt ../nitrofiles/fnt
