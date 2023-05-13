#!/bin/sh

grit whiteball.png -ftb -fh! -gTFF00FF -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img ../nitrofiles/sprite

grit pdemo_bg.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ../nitrofiles/bg

grit pdemo_colmap.png -ftb -fh! -gt -gB8 -mRtp -mLf -p!

for file in *.img.bin; do
    mv -- "$file" "${file%.img.bin}".dat
done

for file in *.map.bin; do
    mv -- "$file" "${file%.map.bin}".cmp
done

mv *.dat *.cmp ../nitrofiles/maps
