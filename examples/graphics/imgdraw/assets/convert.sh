#!/bin/sh

grit bitmap16.png -ftb -fh! -gb -gB16
grit blueball.png -ftb -fh! -gb -gB16

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.img ../nitrofiles/bmp
