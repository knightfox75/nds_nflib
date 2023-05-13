#!/bin/sh

grit bitmap16.png -ftb -fh! -gb -gB16
grit img16_a.png -ftb -fh! -gb -gB16
grit img16_b.png -ftb -fh! -gb -gB16

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.img ../nitrofiles/bmp
