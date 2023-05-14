#!/bin/sh

# The two backgrounds in the same screen share the palette
grit navygrid.png waves512.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLa -pS -Onavygrid

grit flag512.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLa

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

# Both backgrounds need to have a palette, even if it's the same one
cp navygrid.pal waves512.pal

mv *.pal *.img *.map ../nitrofiles/bg
