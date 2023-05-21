#!/bin/sh

grit pointer.png -ftb -fh! -gTFF00FF -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img ../nitrofiles/sprite

grit colmap.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
grit layer3.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ../nitrofiles/bg

grit cmap.png -ftb -fh! -g! -gB8 -mRt -mLf -p!

for file in *.map.bin; do
    mv -- "$file" "${file%.map.bin}".cmp
done

mv *.cmp ../nitrofiles/maps

grit default.png -ftb -fh! -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

for file in *.img; do
    mv -- "$file" "${file%.img}".fnt
done

mv *.pal *.fnt ../nitrofiles/fnt
