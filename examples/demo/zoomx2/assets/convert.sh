#!/bin/sh
grit img16_c.jpg -ftb -fh! -gb -gB16

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.img ../nitrofiles/bmp
