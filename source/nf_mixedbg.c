// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Mixed (tiled and 8-bit bitmap) background functions
// http://www.nightfoxandco.com/

#include <string.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_bitmapbg.h"
#include "nf_mixedbg.h"
#include "nf_tiledbg.h"

void NF_InitMixedBgSys(int screen)
{
    // Define the number of banks of maps and tiles.
    //
    // Tile banks are 16 KB in size. Map banks are 2 KB in size. You can fit 8
    // map banks in a tile bank. For that reason, the number of map banks must
    // be a multiple of 8 banks.
    //
    // By default, use 4 tile banks and 8 map banks.
    NF_BANKS_TILES[screen] = 4;
    NF_BANKS_MAPS[screen] = 8;

    // Set all tile and map blocks as free
    for (int n = 0; n < NF_BANKS_TILES[screen]; n++)
        NF_TILEBLOCKS[screen][n] = 0;

    for (int n = 0; n < NF_BANKS_MAPS[screen]; n++)
        NF_MAPBLOCKS[screen][n] = 0;

    // Reset information of backgrounds
    for (int n = 0; n < 4; n ++)
    {
        NF_TILEDBG_LAYERS[screen][n].tilebase = 0;      // Tileset base
        NF_TILEDBG_LAYERS[screen][n].tileblocks = 0;    // Blocks used by the tileset
        NF_TILEDBG_LAYERS[screen][n].mapbase = 0;       // Map base
        NF_TILEDBG_LAYERS[screen][n].mapblocks = 0;     // Blocks used by the map
        NF_TILEDBG_LAYERS[screen][n].bgwidth = 0;       // Background width
        NF_TILEDBG_LAYERS[screen][n].bgheight = 0;      // Background height
        NF_TILEDBG_LAYERS[screen][n].mapwidth = 0;      // Map width
        NF_TILEDBG_LAYERS[screen][n].mapheight = 0;     // Map height
        NF_TILEDBG_LAYERS[screen][n].bgtype = 0;        // Map type
        NF_TILEDBG_LAYERS[screen][n].bgslot = 0;        // Graphics slot
        NF_TILEDBG_LAYERS[screen][n].blockx = 0;        // Current horizontal map block
        NF_TILEDBG_LAYERS[screen][n].blocky = 0;        // Current vertical map block
        NF_TILEDBG_LAYERS[screen][n].created = false;   // Is the background created?
    }

    // Now reserve as many VRAM banks as needed for maps. Each tile map is as
    // big as 8 map banks.

    // Number of required tile maps to reserve for maps
    u8 r_banks = ((NF_BANKS_MAPS[screen] - 1) / 8) + 1;
    for (int n = 0; n < r_banks; n ++)
        NF_TILEBLOCKS[screen][n] = 128; // Flag them as "map" banks

    if (screen == 0)
    {
        // Top screen, main engine
        REG_DISPCNT |= DISPLAY_BG_EXT_PALETTE;  // Enable extended palettes
        vramSetBankA(VRAM_A_MAIN_BG);           // VRAM_A: Main engine backgrounds (128 KB)
        memset((void *)0x06000000, 0, 131072);  // Clear VRAM_A
        vramSetBankE(VRAM_E_LCD);               // VRAM_E: Extended palettes (32 / 64 KB)
        memset((void *)0x06880000, 0, 32768);   // Clear VRAM_E
        for (int n = 0; n < 4; n++)             // Hide all 4 layers
            NF_HideBg(0, n);
    }
    else
    {
        // Bottom screen, sub engine
        REG_DISPCNT_SUB |= DISPLAY_BG_EXT_PALETTE;  // Enable extended palettes
        vramSetBankC(VRAM_C_SUB_BG);            // VRAM_C: Sub engine backgrounds (128 KB)
        memset((void *)0x06200000, 0, 131072);  // Clear VRAM_C
        vramSetBankH(VRAM_H_LCD);               // VRAM_H: Extended palettes (32 / 64 KB)
        memset((void *)0x06898000, 0, 32768);   // Clear VRAM_H
        for (int n = 0; n < 4; n++)             // Hide all 4 layers
            NF_HideBg(1, n);
    }

    // Initialize the drawing layer for bitmaps (layer 3)
    if (screen == 0)
    {
        // Set layer 3 to 8 bits mode
        REG_BG3CNT = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
        // Reset rotation/scaling
        REG_BG3PA = 1 << 8;
        REG_BG3PB = 0;
        REG_BG3PC = 0;
        REG_BG3PD = 1 << 8;
        NF_ScrollBg(0, 3, 0, 0); // Reset scroll
        NF_ShowBg(0, 3);         // Show layer 3
    }
    else
    {
        // Set layer 3 to 8 bits mode
        REG_BG3CNT_SUB = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
        // Reset rotation/scaling
        REG_BG3PA_SUB = 1 << 8;
        REG_BG3PB_SUB = 0;
        REG_BG3PC_SUB = 0;
        REG_BG3PD_SUB = 1 << 8;
        NF_ScrollBg(1, 3, 0, 0); // Reset scroll
        NF_ShowBg(1, 3);         // Show layer 3
    }
}
