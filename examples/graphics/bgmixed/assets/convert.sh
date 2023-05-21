#!/bin/sh

grit ball.png -ftb -fh! -gTFF00FF -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img ../nitrofiles/sprite

grit img8b_1.png -ftb -fh! -gTFF00FF -gb -gB8
grit img8b_2.png -ftb -fh! -gTFF00FF -gb -gB8

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img ../nitrofiles/bmp

grit bg0.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ../nitrofiles/bg

grit font16.png -ftb -fh! -gTFF00FF -gt -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

for file in *.img; do
    mv -- "$file" "${file%.img}".fnt
done

mv *.pal *.fnt ../nitrofiles/fnt
