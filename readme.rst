##############
NightFox’s Lib
##############

Introduction
============

NightFox’s Lib is a library written in C to make it easier to develop for the
NDS. It depends on libnds, but it is much easier than libnds if the user wants
to use backgrounds, sprites, collision maps, etc.

Features:

- 256 color tilemaps:

  - You can load them from FAT or NitroFS, and you may load up to 32 in RAM.
  - The size must be a multiple of 256 pixels.
  - Extended palettes are supported (up to 16 palettes of 256 colors per bg).
  - VRAM is setup so that you can use 96 KB for tiles and 32 KB for maps per
    screen.
  - "Infinite maps" are supported, using the technique "bank swap", which keeps
    the map in RAM all the time and copies to VRAM only the part that is being
    shown. The only limitation is the tileset size, which must not be over 96 KB
    for all 4 layers.
  - Scroll is supported, and parts of the map are loaded to VRAM as they are
    needed.
  - It is possible to show or hide backgrounds, unload them from RAM, modify
    tiles in the map at runtime, change the palette, etc.

- 256 color sprites:

  - You may load up to 256 objects (128 per screen) and 64 palettes.
  - You can hide sprites, rotate them, change their size, flip them, etc.
  - Animated sprites are supported, with no frame limit. You can choose if the
    frames are stored in VRAM or only the image being shown in that moment.
  - It is possible to modify the palettes at runtime to create visual effects.

- Text engine:

  - Based on tiled backgrounds.
  - It supports 8x8 and 8x16 pixel fonts.
  - 256 color font are supported.
  - You can use fonts already colored or you can define their colors at runtime.
  - You can rotate them by 90 degrees and scroll them.
  - You need to load a font per background layer that will use it.

- Sound:

  - Basic support to load RAW audio files from FAT or NitroFS.
  - It uses libnds to play them.
  - You can load up to 32 sounds to RAM.

- Collision maps:

  - The library supports collision maps that can be edited at runtime.
  - Up to 32 mapx can be loaded at the same time in RAM.

- 8 and 16 bit backgrounds:

  - Useful to display images with a higher detail that tiled maps.
  - There are functions to load them and edit them.
  - You may load up to 16 images in total.
  - There is backbuffer support.

- Media:

  - The library supports loading 8, 16, and 24 bit BMP files.

- 3D sprites:

  - They use the 3D engine of the console to display up to 256 sprites.
  - The maximum size is 1024x1024.
  - You may rotate them, scale them, and define up to 62 transparency
    configurations.

- WiFi:

  - Basic WiFi support.
  - Functions for basic UDP communications.

- Examples:

  - Lots of examples are available to see how to use the library.

- Documentation:

  - Doxygen documentation in English.

- License:

  - The library is licensed under the MIT license.
  - The assets in this repository are licensed under the CC-BY-4.0 license.
  - The examples are licensed under the CC0 license.

Setup instructions for BlocksDS
===============================

1. Clone this repository. If you have followed the getting started guide of
   `BlocksDS <https://blocksds.github.io/docs/>`_, all you have to do is
   go to the folder of NFlib and run:

   .. code:: bash

       make install

2. That's it! Go to the folder of any of the examples and try to build it with:

   .. code:: bash

       make

Note: The build system of the examples in this repository is make. The makefiles
aren't very flexible, and they don't support saving graphics to the filesystem
(you can only inject them as data to the ARM9, which isn't acceptable for big
games).

You can try `ArchitectDS <https://github.com/AntonioND/architectds>`_. This
build system written in Python is more flexible, and it allows you to save your
converted graphics to NitroFS so that your game can grow as much as you want.

Setup instructions for devkitPro
================================

This option is discouraged. The implementation of NitroFS of devkitPro is buggy
and it won't work in accurate emulators like **melonDS**.

1. Clone this repository. Create a symbolic link to it inside the devkitPro
   folder in your system, and call it ``nflib``.

   For example, in Linux, create a symlink so that ``/opt/devkitpro/nflib``
   points to the folder with NFlib:

   .. code:: bash

       ln -sT /path/to/nds_nflib /opt/devkitpro/nflib

   On Windows, the path should be ``C:\devkitpro\nflib``.

   If this doesn't work, copy the folder of NFlib to the devkitPro folder and
   rename it to ``nflib``.

2. Go to the ``nflib`` folder and type this on the terminal:

   .. code:: bash

       make -f Makefile.dkp

3. That's it! Go to the folder of any of the examples and try to build it with:

   .. code:: bash

       make -f Makefile.dkp
