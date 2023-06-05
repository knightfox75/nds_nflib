// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Affine backgrounds functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_affinebg.h"
#include "nf_basic.h"
#include "nf_tiledbg.h"

// Struct that holds informatio about all affine backgrounds
NF_TYPE_AFFINE_BG NF_AFFINE_BG[2][4];

void NF_InitAffineBgSys(int screen)
{
    // Define the number of banks of maps and tiles.
    //
    // Tile banks are 16 KB in size. Map banks are 2 KB in size. You can fit 8
    // map banks in a tile bank. For that reason, the number of map banks must
    // be a multiple of 8 banks.
    //
    // By default, use 8 tile banks and 16 map banks.
    NF_BANKS_TILES[screen] = 8;
    NF_BANKS_MAPS[screen] = 16;

    // Set all tile and map blocks as free
    for (u32 n = 0; n < NF_BANKS_TILES[screen]; n++)
        NF_TILEBLOCKS[screen][n] = 0;
    for (u32 n = 0; n < NF_BANKS_MAPS[screen]; n++)
        NF_MAPBLOCKS[screen][n] = 0;

    // Reset state of all layers
    for (int n = 0; n < 4; n++)
    {
        NF_TILEDBG_LAYERS[screen][n].tilebase = 0;
        NF_TILEDBG_LAYERS[screen][n].tileblocks = 0;
        NF_TILEDBG_LAYERS[screen][n].mapbase = 0;
        NF_TILEDBG_LAYERS[screen][n].mapblocks = 0;
        NF_TILEDBG_LAYERS[screen][n].bgwidth = 0;
        NF_TILEDBG_LAYERS[screen][n].bgheight = 0;
        NF_TILEDBG_LAYERS[screen][n].mapwidth = 0;
        NF_TILEDBG_LAYERS[screen][n].mapheight = 0;
        NF_TILEDBG_LAYERS[screen][n].bgtype = 0;
        NF_TILEDBG_LAYERS[screen][n].bgslot = 0;
        NF_TILEDBG_LAYERS[screen][n].blockx = 0;
        NF_TILEDBG_LAYERS[screen][n].blocky = 0;
        NF_TILEDBG_LAYERS[screen][n].created = false; // Mark as not created
    }

    // Now reserve as many VRAM banks as needed for maps. Each tile map is as
    // big as 8 map banks.

    // Number of required tile maps to reserve for maps
    u32 r_banks = ((NF_BANKS_MAPS[screen] - 1) / 8) + 1;
    for (u32 n = 0; n < r_banks; n++)
        NF_TILEBLOCKS[screen][n] = 128; // Mark as available for maps

    if (screen == 0)
    {
        // Clear VRAM_A (128 KB)
        vramSetBankA(VRAM_A_MAIN_BG);
        memset((void *)0x06000000, 0, 131072);

        // Hide all 4 layers
        for (int n = 0; n < 4; n++)
            NF_HideBg(0, n);
    }
    else
    {
        // Clear VRAM_C (128 KB)
        vramSetBankC(VRAM_C_SUB_BG);
        memset((void *)0x06200000, 0, 131072);

        // Hide all 4 layers
        for (int n = 0; n < 4; n++)
            NF_HideBg(1, n);
    }
}

void NF_LoadAffineBg(const char *file, const char *name, u32 width, u32 height)
{
    // Verify that the background has the right size
    if (!(((width == 256) && (height == 256)) || ((width == 512) && (height == 512))))
        NF_Error(117, name, 0);

    // Look for a free tiled background slot
    u8 slot = 255;
    for (u32 n = 0; n < NF_SLOTS_TBG; n++)
    {
        if (NF_TILEDBG[n].available)
        {
            NF_TILEDBG[n].available = false; // Mark it as used
            slot = n;
            break;
        }
    }

    if (slot == 255) // No free slots, fail
        NF_Error(103, "Tiled Bg", NF_SLOTS_TBG);

    // Free buffers if they were in use
    free(NF_BUFFER_BGMAP[slot]);
    NF_BUFFER_BGMAP[slot] = NULL;
    free(NF_BUFFER_BGTILES[slot]);
    NF_BUFFER_BGTILES[slot] = NULL;
    free(NF_BUFFER_BGPAL[slot]);
    NF_BUFFER_BGPAL[slot] = NULL;

    // File path
    char filename[256];

    // Load .IMG file
    snprintf(filename, sizeof(filename), "%s/%s.img", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_BGTILES[slot], &NF_TILEDBG[slot].tilesize, 0);

    // Verify that the tileset is at most 256 tiles
    if (NF_TILEDBG[slot].tilesize > 16384)
        NF_Error(117, name, 0);

    // Load .MAP file
    snprintf(filename, sizeof(filename), "%s/%s.map", NF_ROOTFOLDER, file);
    FILE *file_id = fopen(filename, "rb");
    if (file_id == NULL) // If the file can't be opened
        NF_Error(101, filename, 0);

    // Get file size
    fseek(file_id, 0, SEEK_END);
    NF_TILEDBG[slot].mapsize = (((ftell(file_id) - 1) >> 10) + 1) << 10; // 1 KB blocks
    rewind(file_id);

    // Allocate space in RAM
    NF_BUFFER_BGMAP[slot] = malloc(NF_TILEDBG[slot].mapsize);
    if (NF_BUFFER_BGMAP[slot] == NULL) // Not enough available RAM
        NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);

    // Read file into RAM
    fread(NF_BUFFER_BGMAP[slot], 1, NF_TILEDBG[slot].mapsize, file_id);
    fclose(file_id);

    // Load .PAL file
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_BGPAL[slot], &NF_TILEDBG[slot].palsize, 256 * 2);

    // Save information of the tiled background
    snprintf(NF_TILEDBG[slot].name, sizeof(NF_TILEDBG[slot].name), "%s", name);

    NF_TILEDBG[slot].width = width;
    NF_TILEDBG[slot].height = height;
}

void NF_CreateAffineBg(int screen, u32 layer, const char *name, u32 wrap)
{
    // Verify that the destination layer is valid
    if ((layer != 2) && (layer != 3))
        NF_Error(118, name, 0);

    u32 slot = 255;

    for (u32 n = 0; n < NF_SLOTS_TBG; n++)
    {
        if (strcmp(name, NF_TILEDBG[n].name) == 0)
        {
            slot = n;
            break;
        }
    }

    if (slot == 255) // The slot hasn't been found, fail
        NF_Error(104, name, 0);

    // If there is already a background in this layer, delete it before
    if (NF_TILEDBG_LAYERS[screen][layer].created)
        NF_DeleteTiledBg(screen, layer);

    // Copy the size of the background
    NF_TILEDBG_LAYERS[screen][layer].bgwidth = NF_TILEDBG[slot].width;
    NF_TILEDBG_LAYERS[screen][layer].bgheight = NF_TILEDBG[slot].height;
    NF_TILEDBG_LAYERS[screen][layer].mapwidth = NF_TILEDBG[slot].width;
    NF_TILEDBG_LAYERS[screen][layer].mapheight = NF_TILEDBG[slot].height;
    NF_TILEDBG_LAYERS[screen][layer].bgtype = 0;
    NF_TILEDBG_LAYERS[screen][layer].bgslot = slot;

    if ((NF_TILEDBG[slot].width == 256) && (NF_TILEDBG[slot].height == 256))
    {
        NF_TILEDBG_LAYERS[screen][layer].mapwidth = 256;
        NF_TILEDBG_LAYERS[screen][layer].mapheight = 256;
        NF_TILEDBG_LAYERS[screen][layer].bgtype = 11;
    }
    else if ((NF_TILEDBG[slot].width == 512) && (NF_TILEDBG[slot].height == 512))
    {
        NF_TILEDBG_LAYERS[screen][layer].mapwidth = 512;
        NF_TILEDBG_LAYERS[screen][layer].mapheight = 512;
        NF_TILEDBG_LAYERS[screen][layer].bgtype = 12;
    }
    else
    {
        NF_Error(117, name, 0);
    }

    // Verify that the tileset is 256 tiles or fewer
    if (NF_TILEDBG[slot].tilesize > 16384)
        NF_Error(117, name, 0);

    // Calculate the required number of blocks
    u32 tilesblocks = ((NF_TILEDBG[slot].tilesize - 1) >> 14) + 1;

    // Determine a location for the new tileset
    u32 counter = 0;
    u32 start = 255;
    for (u32 n = 0; n < NF_BANKS_TILES[screen]; n++)
    {
        if (NF_TILEBLOCKS[screen][n] == 0)
        {
            // If the block is free and this is the first free block, save the
            // start address.
            if (counter == 0)
                start = n;

            // If enough blocks have been found stop the search
            counter++;
            if (counter == tilesblocks)
                n = NF_BANKS_TILES[screen];
        }
        else
        {
            // If the block is not free, reset the counter and try again
            start = 255;
            counter = 0;
        }
    }

    // If no free blocks have been found
    if ((start == 255) || (counter < tilesblocks))
        NF_Error(107, name, tilesblocks);

    // Mark all required map banks as used
    u32 basetiles = start;
    for (u32 n = basetiles; n < (basetiles + tilesblocks); n ++)
        NF_TILEBLOCKS[screen][n] = 255;

    // Calculate the required number of blocks
    u32 mapblocks = ((NF_TILEDBG[slot].mapsize - 1) >> 11) + 1;

    // Determine a location for the new map
    counter = 0;
    start = 255;

    for (u32 n = 0; n < NF_BANKS_MAPS[screen]; n++)
    {
        if (NF_MAPBLOCKS[screen][n] == 0)
        {
            // If the block is free and this is the first free block, save the
            // start address.
            if (counter == 0)
                start = n;

            // If enough blocks have been found stop the search
            counter++;
            if (counter == mapblocks)
                n = NF_BANKS_MAPS[screen];
        }
        else
        {
            // If the block is not free, reset the counter and try again
            start = 255;
            counter = 0;
        }
    }

    // If no free blocks have been found
    if ((start == 255) || (counter < mapblocks))
        NF_Error(108, name, mapblocks);

    // Mark all required map banks as used
    u32 basemap = start;
    for (u32 n = basemap; n < (basemap + mapblocks); n++)
        NF_MAPBLOCKS[screen][n] = 255;

    // Check the size of the background
    s32 bg_size;
    if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 256) &&
        (NF_TILEDBG_LAYERS[screen][layer].mapheight == 256))
    {
        bg_size = BG_RS_32x32;
    }
    else if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 512) &&
             (NF_TILEDBG_LAYERS[screen][layer].mapheight == 512))
    {
        bg_size = BG_RS_64x64;
    }
    else
    {
        NF_Error(117, name, 0);
    }

    // Decide if wrap is enabled or not
    u32 wrap_mode = wrap == 0 ?  BG_WRAP_OFF : BG_WRAP_ON;

    // Setup the background properties
    if (screen == 0)
    {
        switch (layer)
        {
            case 2:
                REG_BG2CNT = BgType_Rotation | bg_size | BG_PRIORITY_2 | BG_COLOR_256 |
                             BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
                break;
            case 3:
                REG_BG3CNT = BgType_Rotation | bg_size | BG_PRIORITY_3 | BG_COLOR_256 |
                             BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
                break;
        }
    }
    else
    {
        switch (layer)
        {
            case 2:
                REG_BG2CNT_SUB = BgType_Rotation | bg_size | BG_PRIORITY_2 | BG_COLOR_256 |
                                 BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
                break;
            case 3:
                REG_BG3CNT_SUB = BgType_Rotation | bg_size | BG_PRIORITY_3 | BG_COLOR_256 |
                                 BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
                break;
        }
    }

    u32 address;

    // Transfer tileset to VRAM
    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basetiles << 14);
    else // VRAM_C
        address = 0x6200000 + (basetiles << 14);
    NF_DmaMemCopy((void *)address, NF_BUFFER_BGTILES[slot], NF_TILEDBG[slot].tilesize);

    // Transfer map to VRAM
    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basemap << 11);
    else // VRAM_C
        address = 0x6200000 + (basemap << 11);
    NF_DmaMemCopy((void *)address, NF_BUFFER_BGMAP[slot], NF_TILEDBG[slot].mapsize);

    // Tranfer palette to palette memory
    if (screen == 0)
        address = 0x05000000; // Main engine standard BG palette
    else
        address = 0x05000400; // Sub engine standard BG palette
    NF_DmaMemCopy((void *)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);

    // Save information
    NF_TILEDBG_LAYERS[screen][layer].tilebase = basetiles;
    NF_TILEDBG_LAYERS[screen][layer].tileblocks = tilesblocks;
    NF_TILEDBG_LAYERS[screen][layer].mapbase = basemap;
    NF_TILEDBG_LAYERS[screen][layer].mapblocks = mapblocks;
    NF_TILEDBG_LAYERS[screen][layer].created = true; // Mark as created

    // Reset affine parameters
    NF_AffineBgTransform(screen, layer, 1 << 8, 1 << 8, 0, 0);
    NF_AffineBgMove(screen, layer, 0, 0, 0);

    // Make the newly created background visible
    NF_ShowBg(screen, layer);
}

void NF_DeleteAffineBg(int screen, u32 layer)
{
    // Verify that the background has been created
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    // Hide background
    NF_HideBg(screen, layer);

    // Clear tileset from VRAM
    u32 basetiles = NF_TILEDBG_LAYERS[screen][layer].tilebase;
    u32 tilesize = NF_TILEDBG_LAYERS[screen][layer].tileblocks << 14;
    u32 address;

    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basetiles << 14);
    else // VRAM_C
        address = 0x6200000 + (basetiles << 14);

    memset((void *)address, 0, tilesize);

    // Clear map from VRAM
    u32 basemap = NF_TILEDBG_LAYERS[screen][layer].mapbase;
    u32 mapsize = NF_TILEDBG_LAYERS[screen][layer].mapblocks << 11;

    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basemap << 11);
    else // VRAM_C
        address = 0x6200000 + (basemap << 11);

    memset((void *)address, 0, mapsize);

    // Mark as free all the tile banks used by this background
    tilesize = (basetiles + NF_TILEDBG_LAYERS[screen][layer].tileblocks);
    for (u32 n = basetiles; n < tilesize; n++)
        NF_TILEBLOCKS[screen][n] = 0;

    // Mark as free all the map banks used by this background
    mapsize = basemap + NF_TILEDBG_LAYERS[screen][layer].mapblocks;
    for (u32 n = basemap; n < mapsize; n++)
        NF_MAPBLOCKS[screen][n] = 0;

    // Clear information of the background
    NF_TILEDBG_LAYERS[screen][layer].tilebase = 0;
    NF_TILEDBG_LAYERS[screen][layer].tileblocks = 0;
    NF_TILEDBG_LAYERS[screen][layer].mapbase = 0;
    NF_TILEDBG_LAYERS[screen][layer].mapblocks = 0;
    NF_TILEDBG_LAYERS[screen][layer].bgwidth = 0;
    NF_TILEDBG_LAYERS[screen][layer].bgheight = 0;
    NF_TILEDBG_LAYERS[screen][layer].mapwidth = 0;
    NF_TILEDBG_LAYERS[screen][layer].mapheight = 0;
    NF_TILEDBG_LAYERS[screen][layer].bgtype = 0;
    NF_TILEDBG_LAYERS[screen][layer].bgslot = 0;
    NF_TILEDBG_LAYERS[screen][layer].blockx = 0;
    NF_TILEDBG_LAYERS[screen][layer].blocky = 0;
    NF_TILEDBG_LAYERS[screen][layer].created = false; // Mark as not created
}

void NF_AffineBgTransform(int screen, u32 layer, s32 x_scale, s32 y_scale,
                          s32 x_tilt, s32 y_tilt)
{
    if (screen == 0)
    {
        switch (layer)
        {
            case 2:
                REG_BG2PA = x_scale;
                REG_BG2PB = x_tilt;
                REG_BG2PC = y_tilt;
                REG_BG2PD = y_scale;
                break;
            case 3:
                REG_BG3PA = x_scale;
                REG_BG3PB = x_tilt;
                REG_BG3PC = y_tilt;
                REG_BG3PD = y_scale;
                break;
        }
    }
    else
    {
        switch (layer)
        {
            case 2:
                REG_BG2PA_SUB = x_scale;
                REG_BG2PB_SUB = x_tilt;
                REG_BG2PC_SUB = y_tilt;
                REG_BG2PD_SUB = y_scale;
                break;
            case 3:
                REG_BG3PA_SUB = x_scale;
                REG_BG3PB_SUB = x_tilt;
                REG_BG3PC_SUB = y_tilt;
                REG_BG3PD_SUB = y_scale;
                break;
        }
    }

    // Save values
    NF_AFFINE_BG[screen][layer].x_scale = x_scale;
    NF_AFFINE_BG[screen][layer].x_tilt = x_tilt;
    NF_AFFINE_BG[screen][layer].y_tilt = y_tilt;
    NF_AFFINE_BG[screen][layer].y_scale = y_scale;
}

void NF_AffineBgMove(int screen, u32 layer, s32 x, s32 y, s32 angle)
{
    // Based on the original function of libnds by Dovoto and WinterMute

    // Limit angle
    if (angle < -2048)
        angle += 2048;
    if (angle > 2048)
        angle -= 2048;

    angle = -angle << 4; // Switch from base 2048 to base 32768

    s32 angle_sin = sinLerp(angle);
    s32 angle_cos = cosLerp(angle);

    // Calculate and apply transformation matrix
    s32 pa = (angle_cos * NF_AFFINE_BG[screen][layer].x_scale) >> 12;
    s32 pb = (-angle_sin * NF_AFFINE_BG[screen][layer].x_scale) >> 12;
    s32 pc = (angle_sin * NF_AFFINE_BG[screen][layer].y_scale) >> 12;
    s32 pd = (angle_cos * NF_AFFINE_BG[screen][layer].y_scale) >> 12;

    NF_AffineBgTransform(screen, layer, pa, pd, pb, pc);

    // Calculate the position of the background
    s32 pos_x = (x << 8) -
                    (((pa * (NF_AFFINE_BG[screen][layer].x_center << 8)) +
                      (pb * (NF_AFFINE_BG[screen][layer].y_center << 8))) >> 8);
    s32 pos_y = (y << 8) -
                    (((pc * (NF_AFFINE_BG[screen][layer].x_center << 8)) +
                      (pd * (NF_AFFINE_BG[screen][layer].y_center << 8))) >> 8);

    // Set the position of the center
    if (screen == 0)
    {
        switch (layer)
        {
            case 2:
                REG_BG2X = pos_x;
                REG_BG2Y = pos_y;
                break;
            case 3:
                REG_BG3X = pos_x;
                REG_BG3Y = pos_y;
                break;
        }
    }
    else
    {
        switch (layer)
        {
            case 2:
                REG_BG2X_SUB = pos_x;
                REG_BG2Y_SUB = pos_y;
                break;
            case 3:
                REG_BG3X_SUB = pos_x;
                REG_BG3Y_SUB = pos_y;
                break;
        }
    }

    // Save parameters
    NF_AFFINE_BG[screen][layer].angle = angle;
    NF_AFFINE_BG[screen][layer].x = x;
    NF_AFFINE_BG[screen][layer].y = y;
}
