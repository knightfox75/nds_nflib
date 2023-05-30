// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Tiled background functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_tiledbg.h"

// Tile and map banks. Each tile bank is 16 KB is size, each map bank is 2 KB in
// size. Each tile bank can hold 8 map banks.
u8 NF_BANKS_TILES[2];
u8 NF_BANKS_MAPS[2];

// Buffers that store tilesets, maps and palettes
char *NF_BUFFER_BGTILES[NF_SLOTS_TBG];
char *NF_BUFFER_BGMAP[NF_SLOTS_TBG];
char *NF_BUFFER_BGPAL[NF_SLOTS_TBG];

// Information about backgrounds loaded to RAM
NF_TYPE_TBG_INFO NF_TILEDBG[NF_SLOTS_TBG];
// Information about backgrounds shown in the screen
NF_TYPE_TBGLAYERS_INFO NF_TILEDBG_LAYERS[2][4];

// Struct that keeps information about extended palettes
NF_TYPE_EXBGPAL_INFO NF_EXBGPAL[NF_SLOTS_EXBGPAL];

// Arrays of free tile and map blocks
u8 NF_TILEBLOCKS[2][NF_MAX_BANKS_TILES];
u8 NF_MAPBLOCKS[2][NF_MAX_BANKS_MAPS];

void NF_InitTiledBgBuffers(void)
{
    // Tiled background buffers
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
        NF_BUFFER_BGTILES[n] = NULL;
        NF_BUFFER_BGMAP[n] = NULL;
        NF_BUFFER_BGPAL[n] = NULL;
        snprintf(NF_TILEDBG[n].name, sizeof(NF_TILEDBG[n].name), "xxxNONAMExxx");
        NF_TILEDBG[n].tilesize = 0;
        NF_TILEDBG[n].mapsize = 0;
        NF_TILEDBG[n].palsize = 0;
        NF_TILEDBG[n].width = 0;
        NF_TILEDBG[n].height = 0;
        NF_TILEDBG[n].available = true; // Mark as not used
    }

    // Extended palette buffers
    for (int n = 0; n < NF_SLOTS_EXBGPAL; n++)
    {
        NF_EXBGPAL[n].buffer = NULL;
        NF_EXBGPAL[n].palsize = 0;
        NF_EXBGPAL[n].inuse = false;
    }
}

void NF_ResetTiledBgBuffers(void)
{
    // Free any buffer in use
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
        free(NF_BUFFER_BGTILES[n]);
        free(NF_BUFFER_BGMAP[n]);
        free(NF_BUFFER_BGPAL[n]);
    }
    for (int n = 0; n < NF_SLOTS_EXBGPAL; n++)
        NF_EXBGPAL[n].buffer = NULL;

    // Reset variables
    NF_InitTiledBgBuffers();
}

void NF_InitTiledBgSys(int screen)
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
    u8 r_banks = ((NF_BANKS_MAPS[screen] - 1) / 8) + 1;
    for (u32 n = 0; n < r_banks; n++)
        NF_TILEBLOCKS[screen][n] = 128; // Mark as available for maps

    if (screen == 0)
    {
        // Enable extended palettes
        REG_DISPCNT |= DISPLAY_BG_EXT_PALETTE;

        // Clear VRAM_A and VRAM_E
        vramSetBankA(VRAM_A_MAIN_BG);
        memset((void *)0x06000000, 0, 131072);
        vramSetBankE(VRAM_E_LCD);
        memset((void *)0x06880000, 0, 32768);

        // Hide all 4 layers
        for (int n = 0; n < 4; n++)
            NF_HideBg(0, n);
    }
    else
    {
        // Enable extended palettes
        REG_DISPCNT_SUB |= DISPLAY_BG_EXT_PALETTE;

        // Clear VRAM_C and VRAM_H
        vramSetBankC(VRAM_C_SUB_BG);
        memset((void *)0x06200000, 0, 131072);
        vramSetBankH(VRAM_H_LCD);
        memset((void *)0x06898000, 0, 32768);

        // Hide all 4 layers
        for (int n = 0; n < 4; n++)
            NF_HideBg(1, n);
    }
}

void NF_LoadTiledBg(const char *file, const char *name, u32 width, u32 height)
{
    // Look for a free slot
    int slot = 255;
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
        if (NF_TILEDBG[n].available)
        {
            NF_TILEDBG[n].available = false; // Mark it as in use
            slot = n;
            break;
        }
    }
    // If there are no free slots, fail
    if (slot == 255)
        NF_Error(103, "Tiled Bg", NF_SLOTS_TBG);

    // The size of the background must be a multiple of 256 pixels (32 tiles)
    if (((width % 256) != 0) || ((height % 256) != 0))
        NF_Error(115, file, 0);

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

    // Load .MAP file
    snprintf(filename, sizeof(filename), "%s/%s.map", NF_ROOTFOLDER, file);
    FILE *file_id = fopen(filename, "rb");
    if (file_id == NULL)
        NF_Error(101, filename, 0);

    // Get file size and pad it to a multiple of 2 KB
    fseek(file_id, 0, SEEK_END);
    NF_TILEDBG[slot].mapsize = (((ftell(file_id) - 1) >> 11) + 1) << 11;
    rewind(file_id);

    // Allocate space in RAM and zero it in case there is padding
    NF_BUFFER_BGMAP[slot] = calloc (NF_TILEDBG[slot].mapsize, sizeof(char));
    if (NF_BUFFER_BGMAP[slot] == NULL)
        NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);

    // Load file to RAM
    fread(NF_BUFFER_BGMAP[slot], 1, NF_TILEDBG[slot].mapsize, file_id);
    fclose(file_id);

    // Load .PAL file and pad it to 256 colors
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_BGPAL[slot], &NF_TILEDBG[slot].palsize, 256 * 2);

    // Save background information
    snprintf(NF_TILEDBG[slot].name, sizeof(NF_TILEDBG[slot].name), "%s", name);
    NF_TILEDBG[slot].width = width;
    NF_TILEDBG[slot].height = height;
}

void NF_LoadTilesForBg(const char *file, const char *name, u32 width, u32 height,
                       u32 tile_start, u32 tile_end)
{
    // Look for a free slot
    int slot = 255;
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
        if (NF_TILEDBG[n].available)
        {
            NF_TILEDBG[n].available = false; // Mark it as in use
            slot = n;
            break;
        }
    }
    // There are no free slots, fail
    if (slot == 255)
        NF_Error(103, "Tiled Bg", NF_SLOTS_TBG);

    // The size of the background must be a multiple of 256 pixels (32 tiles)
    if (((width % 256) != 0) || ((height % 256) != 0))
        NF_Error(115, file, 0);

    // Free the buffers if they were in use
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
    FILE *file_id = fopen(filename, "rb");
    if (file_id == NULL)
        NF_Error(101, filename, 0);

    // Get file size
    fseek(file_id, 0, SEEK_END);
    u32 tile_size = ftell(file_id);
    rewind(file_id);

    // Calculate the actual size to load. Each size is 64 bytes.
    NF_TILEDBG[slot].tilesize = ((tile_end - tile_start) + 1) * 64;

    // If the size to load is bigger than the file, fail
    if (((tile_end + 1) * 64) > tile_size)
        NF_Error(106, "tile number", tile_size / 64);

    // Allocate space in RAM
    NF_BUFFER_BGTILES[slot] = malloc(NF_TILEDBG[slot].tilesize);
    if (NF_BUFFER_BGTILES[slot] == NULL)
        NF_Error(102, NULL, NF_TILEDBG[slot].tilesize);

    // Start loading from the first tile requested by the caller
    fseek(file_id, tile_start * 64, SEEK_SET);

    // Read file to RAM
    fread(NF_BUFFER_BGTILES[slot], 1, NF_TILEDBG[slot].tilesize, file_id);
    fclose(file_id);


    // Create an empty .MAP file in RAM
    NF_TILEDBG[slot].mapsize = ((width / 8) * (height / 8)) * 2;

    // Allocate space in RAM and zero it as the initial state of the map
    NF_BUFFER_BGMAP[slot] = calloc(NF_TILEDBG[slot].mapsize, sizeof(u8));
    if (NF_BUFFER_BGMAP[slot] == NULL)
        NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);

    // Load .PAL file
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_BGPAL[slot], &NF_TILEDBG[slot].palsize, 256 * 2);

    // Save background information
    snprintf(NF_TILEDBG[slot].name, sizeof(NF_TILEDBG[slot].name), "%s", name);
    NF_TILEDBG[slot].width = width;
    NF_TILEDBG[slot].height = height;
}

void NF_UnloadTiledBg(const char *name)
{
    // Look for the requested slot
    u32 slot = 255;
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
        if (strcmp(name, NF_TILEDBG[n].name) == 0)
        {
            slot = n;
            break;
        }
    }
    // Fail if it wasn't found
    if (slot == 255)
        NF_Error(104, name, 0);

    // Free buffers
    free(NF_BUFFER_BGMAP[slot]);
    NF_BUFFER_BGMAP[slot] = NULL;
    free(NF_BUFFER_BGTILES[slot]);
    NF_BUFFER_BGTILES[slot] = NULL;
    free(NF_BUFFER_BGPAL[slot]);
    NF_BUFFER_BGPAL[slot] = NULL;

    // Reset state of the background
    snprintf(NF_TILEDBG[slot].name, sizeof(NF_TILEDBG[slot].name), "xxxNONAMExxx");
    NF_TILEDBG[slot].tilesize = 0;
    NF_TILEDBG[slot].mapsize = 0;
    NF_TILEDBG[slot].palsize = 0;
    NF_TILEDBG[slot].width = 0;
    NF_TILEDBG[slot].height = 0;
    NF_TILEDBG[slot].available = true; // Mark as available
}

void NF_CreateTiledBg(int screen, u32 layer, const char *name)
{
    // Look for the requested background slot
    u32 slot = 255;
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
        if (strcmp(name, NF_TILEDBG[n].name) == 0)
        {
            slot = n;
            break;
        }
    }
    // If the name wasn't found, fail
    if (slot == 255)
        NF_Error(104, name, 0);

    // Delete any background that is already in this layer
    if (NF_TILEDBG_LAYERS[screen][layer].created)
        NF_DeleteTiledBg(screen, layer);

    // Set background information
    NF_TILEDBG_LAYERS[screen][layer].bgwidth = NF_TILEDBG[slot].width;
    NF_TILEDBG_LAYERS[screen][layer].bgheight = NF_TILEDBG[slot].height;
    NF_TILEDBG_LAYERS[screen][layer].mapwidth = NF_TILEDBG[slot].width;
    NF_TILEDBG_LAYERS[screen][layer].mapheight = NF_TILEDBG[slot].height;
    NF_TILEDBG_LAYERS[screen][layer].bgtype = 0; // Set regular type for now
    NF_TILEDBG_LAYERS[screen][layer].bgslot = slot;

    // If the map is infinite determine the type and size of the hardware map
    if ((NF_TILEDBG[slot].width >= 512) && (NF_TILEDBG[slot].height <= 256))
    {
        // >512 x n
        NF_TILEDBG_LAYERS[screen][layer].mapwidth = 512;
        NF_TILEDBG_LAYERS[screen][layer].mapheight = NF_TILEDBG[slot].height;
        NF_TILEDBG_LAYERS[screen][layer].bgtype = 1;
    }
    else if ((NF_TILEDBG[slot].width <= 256) && (NF_TILEDBG[slot].height >= 512))
    {
        // n x >512
        NF_TILEDBG_LAYERS[screen][layer].mapwidth = NF_TILEDBG[slot].width;
        NF_TILEDBG_LAYERS[screen][layer].mapheight = 512;
        NF_TILEDBG_LAYERS[screen][layer].bgtype = 2;
    }
    else if ((NF_TILEDBG[slot].width >= 512) && (NF_TILEDBG[slot].height >= 512))
    {
        // >512 x >512
        NF_TILEDBG_LAYERS[screen][layer].mapwidth = 512;
        NF_TILEDBG_LAYERS[screen][layer].mapheight = 512;
        NF_TILEDBG_LAYERS[screen][layer].bgtype = 3;
    }

    // Look for enough free contiguous tiles blocks in VRAM
    u32 tilesblocks = ((NF_TILEDBG[slot].tilesize - 1) >> 14) + 1;

    u32 counter = 0;
    u32 start = 255;
    for (u32 n = 0; n < NF_BANKS_TILES[screen]; n++)
    {
        // Is this block free?
        if (NF_TILEBLOCKS[screen][n] == 0)
        {
            // If this is the first free block, mark it as the start
            if (counter == 0)
                start = n;

            counter++;

            // If enough blocks have been found, exit
            if (counter == tilesblocks)
                break;
        }
        else
        {
            // The block isn't free, clear the starting point and counter
            start = 255;
            counter = 0;
        }
    }

    // Fail if not enough contiguous free blocks have been found
    if ((start == 255) || (counter < tilesblocks))
        NF_Error(107, name, tilesblocks);

    // Mark all required tile banks as used
    u32 basetiles = start;
    for (u32 n = basetiles; n < (basetiles + tilesblocks); n++)
        NF_TILEBLOCKS[screen][n] = 255;

    // Look for enough free contiguous map blocks in VRAM
    u32 mapblocks;

    if (NF_TILEDBG_LAYERS[screen][layer].bgtype == 0)
    {
        // If this is a normal map (<= 512)
        mapblocks = ((NF_TILEDBG[slot].mapsize - 1) >> 11) + 1;
    }
    else
    {
        // If this is an infinite map (> 512)
        u32 mapsize = ((NF_TILEDBG_LAYERS[screen][layer].mapwidth / 8) *
                       (NF_TILEDBG_LAYERS[screen][layer].mapheight / 8)) * 2;
        mapblocks = ((mapsize - 1) >> 11) + 1;
    }

    counter = 0;
    start = 255;
    for (u32 n = 0; n < NF_BANKS_MAPS[screen]; n++)
    {
        // Is this block free?
        if (NF_MAPBLOCKS[screen][n] == 0)
        {
            // If this is the first free block, mark it as the start
            if (counter == 0)
                start = n;

            counter++;

            // If enough blocks have been found, exit
            if (counter == mapblocks)
                break;
        }
        else
        {
            // The block isn't free, clear the starting point and counter
            start = 255;
            counter = 0;
        }
    }

    // Fail if not enough contiguous free blocks have been found
    if ((start == 255) || (counter < mapblocks))
        NF_Error(108, name, mapblocks);

    // Mark the required map banks as used
    u32 basemap = start;
    for (u32 n = basemap; n < (basemap + mapblocks); n++)
        NF_MAPBLOCKS[screen][n] = 255;

    // Get background size
    u32 bg_size = 0;

    if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 256) &&
        (NF_TILEDBG_LAYERS[screen][layer].mapheight == 256))
    {
        bg_size = BG_32x32;
    }
    else if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 512) &&
             (NF_TILEDBG_LAYERS[screen][layer].mapheight == 256))
    {
        bg_size = BG_64x32;
    }
    else if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 256) &&
             (NF_TILEDBG_LAYERS[screen][layer].mapheight == 512))
    {
        bg_size = BG_32x64;
    }
    else if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 512) &&
             (NF_TILEDBG_LAYERS[screen][layer].mapheight == 512))
    {
        bg_size = BG_64x64;
    }
    else
    {
        // TODO: Fail
    }

    // Setup the hardware layer
    if (screen == 0)
    {
        switch (layer)
        {
            case 0:
                REG_BG0CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_0 |
                             BG_PALETTE_SLOT0 | BG_COLOR_256 |
                             BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
            case 1:
                REG_BG1CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_1 |
                             BG_PALETTE_SLOT1 | BG_COLOR_256 |
                             BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
            case 2:
                REG_BG2CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_2 |
                             BG_COLOR_256 |
                             BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
            case 3:
                REG_BG3CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_3 |
                             BG_COLOR_256 |
                             BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
        }
    }
    else
    {
        switch (layer)
        {
            case 0:
                REG_BG0CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_0 |
                                 BG_PALETTE_SLOT0 | BG_COLOR_256 |
                                 BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
            case 1:
                REG_BG1CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_1 |
                                 BG_PALETTE_SLOT1 | BG_COLOR_256 |
                                 BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
            case 2:
                REG_BG2CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_2 |
                                 BG_COLOR_256 |
                                 BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
            case 3:
                REG_BG3CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_3 |
                                 BG_COLOR_256 |
                                 BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
                break;
        }
    }

    u32 address;

    // Copy tileset to VRAM
    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basetiles << 14);
    else // VRAM_C
        address = 0x6200000 + (basetiles << 14);
    NF_DmaMemCopy((void *)address, NF_BUFFER_BGTILES[slot], NF_TILEDBG[slot].tilesize);

    // Copy map to VRAM
    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basemap << 11);
    else // VRAM_C
        address = 0x6200000 + (basemap << 11);

    if (NF_TILEDBG_LAYERS[screen][layer].bgtype == 0)
    {
        // Regular 32x32 map, copy it in one go
        NF_DmaMemCopy((void *)address, NF_BUFFER_BGMAP[slot], NF_TILEDBG[slot].mapsize);
    }
    else
    {
        // Infinite map, check the type
        switch (NF_TILEDBG_LAYERS[screen][layer].bgtype)
        {
            case 1: // >512 x 256
                // Blocks A and B (32x32) + (32x32) (2 KB x 2 = 4 KB)
                NF_DmaMemCopy((void *)address, NF_BUFFER_BGMAP[slot], 4096);
                break;

            case 2:  // 256 x >512
                // Block A (32x64) (2 KB x 2 = 4 KB)
                NF_DmaMemCopy((void *)address, NF_BUFFER_BGMAP[slot], 4096);
                break;

            case 3: // >512 x >512
                // Blocks A and B (32x32) + (32x32) (2 KB x 2 = 4 KB)
                NF_DmaMemCopy((void *)address, NF_BUFFER_BGMAP[slot], 4096);
                // Blocks (+4096) C and D (32x32) + (32x32) (2 KB x 2 = 4 KB)
                u32 offset =
                    (((NF_TILEDBG_LAYERS[screen][layer].bgwidth - 1) >> 8) + 1) << 11;
                NF_DmaMemCopy((void *)(address + 4096),
                              NF_BUFFER_BGMAP[slot] + offset, 4096);
                break;
        }
    }

    // Copy palette to VRAM
    if (screen == 0)
    {
        vramSetBankE(VRAM_E_LCD);
        address = 0x06880000 + (layer << 13);
        NF_DmaMemCopy((void *)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
        vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    }
    else
    {
        vramSetBankH(VRAM_H_LCD);
        address = 0x06898000 + (layer << 13);
        NF_DmaMemCopy((void *)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    }

    // Store background information
    NF_TILEDBG_LAYERS[screen][layer].tilebase = basetiles;
    NF_TILEDBG_LAYERS[screen][layer].tileblocks = tilesblocks;
    NF_TILEDBG_LAYERS[screen][layer].mapbase = basemap;
    NF_TILEDBG_LAYERS[screen][layer].mapblocks = mapblocks;
    NF_TILEDBG_LAYERS[screen][layer].created = true; // Mark as created

    // Reset scroll
    NF_ScrollBg(screen, layer, 0, 0);

    // Show the background that contains the newly created layer
    NF_ShowBg(screen, layer);
}

void NF_DeleteTiledBg(int screen, u32 layer)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    NF_HideBg(screen, layer);

    u32 address;

    // Clear the tileset from VRAM
    u32 basetiles = NF_TILEDBG_LAYERS[screen][layer].tilebase;
    u32 tilesize = NF_TILEDBG_LAYERS[screen][layer].tileblocks << 14;
    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basetiles << 14);
    else // VRAM_C
        address = 0x6200000 + (basetiles << 14);
    memset((void *)address, 0, tilesize);

    // Clear the map from VRAM
    u32 basemap = NF_TILEDBG_LAYERS[screen][layer].mapbase;
    u32 mapsize = NF_TILEDBG_LAYERS[screen][layer].mapblocks << 11;
    if (screen == 0) // VRAM_A
        address = 0x6000000 + (basemap << 11);
    else // VRAM_C
        address = 0x6200000 + (basemap << 11);
    memset((void *)address, 0, mapsize);

    // Delete the palette from VRAM
    if (screen == 0)
    {
        vramSetBankE(VRAM_E_LCD); // Let the CPU access VRAM_E
        address = 0x06880000 + (layer << 13);
        memset((void *)address, 0, 8192);
        vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    }
    else
    {
        vramSetBankH(VRAM_H_LCD); // Let the CPU access VRAM_H
        address = 0x06898000 + (layer << 13);
        memset((void *)address, 0, 8192);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    }

    // Mark as free the tile and map banks used by this background
    tilesize = (basetiles + NF_TILEDBG_LAYERS[screen][layer].tileblocks);
    for (u32 n = basetiles; n < tilesize; n++)
        NF_TILEBLOCKS[screen][n] = 0;

    mapsize = (basemap + NF_TILEDBG_LAYERS[screen][layer].mapblocks);
    for (u32 n = basemap; n < mapsize; n++)
        NF_MAPBLOCKS[screen][n] = 0;

    // Reset layer information
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
    NF_TILEDBG_LAYERS[screen][layer].created = false; // Mark as not in use
}

u32 NF_GetTileMapAddress(int screen, u32 layer, u32 tile_x, u32 tile_y)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    // Get the map size in tiles
    u32 size_x = NF_TILEDBG_LAYERS[screen][layer].bgwidth / 8;
    u32 size_y = NF_TILEDBG_LAYERS[screen][layer].bgheight / 8;

    // Check bounds
    if (tile_x > size_x)
        NF_Error(106, "Tile X", size_x);
    if (tile_y > size_y)
        NF_Error(106, "Tile Y", size_y);

    // Maps bigger than 32x32 are arranged in 32x32 blocks. This calculates the
    // 32x32 block where the tile is.
    u32 block_x = tile_x / 32;
    u32 block_y = tile_y / 32;

    // Offset to the start of the block
    u32 row_size = (size_x & ~31) * 32;
    u32 scr_y = block_y * row_size;
    u32 scr_x = (block_x * 32) * 32;

    // Position inside the block
    u32 tls_y = (tile_y - (block_y * 32)) * 32;
    u32 tls_x = tile_x - (block_x * 32);

    return (scr_y + scr_x + tls_y + tls_x) * 2;
}

u32 NF_GetTileOfMap(int screen, u32 layer, u32 tile_x, u32 tile_y)
{
    u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

    u32 lobyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + address);
    u32 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

    return (hibyte << 8) | lobyte;
}

void NF_SetTileOfMap(int screen, u32 layer, u32 tile_x, u32 tile_y, u32 tile)
{
    u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

    u32 hibyte = (tile >> 8) & 0xff;
    u32 lobyte = tile & 0xff;

    *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + address) = lobyte;
    *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = hibyte;
}

void NF_UpdateVramMap(int screen, u32 layer)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    // Calculate base address of the map
    u32 address;

    if (screen == 0) // VRAM_A
        address = 0x6000000 + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11);
    else // VRAM_C
        address = 0x6200000 + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11);

    u32 off_x, off_y;
    u32 rowsize;

    switch (NF_TILEDBG_LAYERS[screen][layer].bgtype)
    {
        case 0: // 256x256 - Block A (32x32) (2 KB)
            // Copy block A
            NF_DmaMemCopy((void *)address,
                    NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot],
                    2048);
            break;

        case 1: // 512x256 - Blocks A and B (32x32) + (32x32) (2 KB x 2 = 4 KB)
            // Calculate offset
            off_x = NF_TILEDBG_LAYERS[screen][layer].blockx << 11;

            // Copy blocks A and B
            NF_DmaMemCopy((void *)address,
                    NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + off_x,
                    4096);
            break;


        case 2: // 256x512 - Block A (32x64) (2 KB x 2 = 4 KB)
            // Calculate offset
            off_y = NF_TILEDBG_LAYERS[screen][layer].blocky << 11;

            // Copy blocks A and B
            NF_DmaMemCopy((void *)address,
                    NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + off_y,
                    4096);
            break;

        case 3: // >512x>512
            // Calculate row size
            rowsize = ((((NF_TILEDBG_LAYERS[screen][layer].bgwidth - 1) >> 8)) + 1) << 11;

            // Calculate offsets
            off_x = (NF_TILEDBG_LAYERS[screen][layer].blocky * rowsize) +
                    (NF_TILEDBG_LAYERS[screen][layer].blockx << 11);
            off_y = off_x + rowsize;

            // Copy blocks A and B (32x32) + (32x32) (2 KB x 2 = 4KB)
            NF_DmaMemCopy((void *)address,
                    NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + off_x,
                    4096);

            // Copy blocks C (+4096) and D (32x32) + (32x32) (2 KB x 2 = 4KB)
            NF_DmaMemCopy((void *)(address + 4096),
                    NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + off_y,
                    4096);
            break;
    }
}

void NF_BgSetPalColor(int screen, u32 layer, u32 number, u32 r, u32 g, u32 b)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    // Pack RGB value
    u32 rgb = r | (g << 5) | (b << 10);

    // Modify palette in VRAM
    if (screen == 0)
    {
        vramSetBankE(VRAM_E_LCD); // Let the CPU access VRAM_E
        u32 address = 0x06880000 + (layer << 13) + (number * 2);
        *((u16*)address) = rgb;
        vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    }
    else
    {
        vramSetBankH(VRAM_H_LCD); // Let the CPU access VRAM_H
        u32 address = 0x06898000 + (layer << 13)  + (number * 2);
        *((u16*)address) = rgb;
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    }
}

void NF_BgEditPalColor(int screen, u32 layer, u32 number, u32 r, u32 g, u32 b)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    // Pack RGB value
    u32 rgb = r | (g << 5) | (b << 10);

    // Write it to the palette in RAM
    u32 hibyte = (rgb >> 8) & 0xFF;
    u32 lobyte = rgb & 0xFF;

    *(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (number * 2)) = lobyte;
    *(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + ((number * 2) + 1)) = hibyte;
}

void NF_BgUpdatePalette(int screen, u32 layer)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    // Get the RAM palette slot
    u32 slot = NF_TILEDBG_LAYERS[screen][layer].bgslot;

    // Transfer palette to VRAM
    if (screen == 0)
    {
        vramSetBankE(VRAM_E_LCD); // Let the CPU access VRAM_E
        u32 address = 0x06880000 + (layer << 13);
        NF_DmaMemCopy((void *)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
        vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    }
    else
    {
        vramSetBankH(VRAM_H_LCD); // Let the CPU access VRAM_H
        u32 address = 0x06898000 + (layer << 13);
        NF_DmaMemCopy((void *)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    }
}

void NF_BgGetPalColor(int screen, u32 layer, u32 number, u8 *r, u8 *g, u8 *b)
{
    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    u32 lobyte = *(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (number * 2));
    u32 hibyte = *(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + ((number * 2) + 1));

    // Get the combined RGB value
    u32 rgb = (hibyte << 8) | lobyte;

    // Calculate RGB components
    *r = rgb & 0x1F;
    *g = (rgb >> 5) & 0x1F;
    *b = (rgb >> 10) & 0x1F;
}

u32 NF_GetTilePal(int screen, u32 layer, u32 tile_x, u32 tile_y)
{
    u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

    // Get the top byte of the map entry, which contains the palette
    u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

    return (hibyte >> 4) & 0x0F;
}

void NF_SetTilePal(int screen, u32 layer, u32 tile_x, u32 tile_y, u32 pal)
{
    u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

    // Extract the top 8 bits of the map entry
    u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

    // Preserve all bits but the palette and set the new palette
    u8 data = (hibyte & 0x0F) | (pal << 4);

    *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = data;
}

void NF_LoadExBgPal(const char *file, u32 slot)
{
    if (slot > (NF_SLOTS_EXBGPAL - 1))
        NF_Error(106, "ExBgPal", NF_SLOTS_EXBGPAL - 1);

    // Free the buffer if it was in use
    free(NF_EXBGPAL[slot].buffer);
    NF_EXBGPAL[slot].buffer = NULL;

    // Load .PAL file
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_EXBGPAL[slot].buffer, &NF_EXBGPAL[slot].palsize, 256 * 2);

    // Mark this slot as unused
    NF_EXBGPAL[slot].inuse = true;
}

void NF_UnloadExBgPal(u32 slot)
{
    if (slot > (NF_SLOTS_EXBGPAL - 1))
        NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));

    if (!NF_EXBGPAL[slot].inuse)
        NF_Error(110, "ExBgPal", slot);

    // Free the buffer
    free(NF_EXBGPAL[slot].buffer);
    NF_EXBGPAL[slot].buffer = NULL;

    NF_EXBGPAL[slot].palsize = 0;

    // Mark the slot as unused
    NF_EXBGPAL[slot].inuse = false;
}

void NF_VramExBgPal(int screen, u32 layer, u32 id, u32 slot)
{
    if (slot > (NF_SLOTS_EXBGPAL - 1))
        NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));

    if (!NF_EXBGPAL[slot].inuse)
        NF_Error(110, "ExBgPal", slot);

    // Transfer palette to VRAM
    if (screen == 0)
    {
        vramSetBankE(VRAM_E_LCD); // Allow CPU accesses
        u32 address = 0x06880000 + (layer << 13) + (slot * 256 * 2);
        NF_DmaMemCopy((void *)address, NF_EXBGPAL[id].buffer, NF_EXBGPAL[id].palsize);
        vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    }
    else
    {
        vramSetBankH(VRAM_H_LCD); // Allow CPU accesses
        u32 address = 0x06898000 + (layer << 13) + (slot * 256 * 2);
        NF_DmaMemCopy((void *)address, NF_EXBGPAL[id].buffer, NF_EXBGPAL[id].palsize);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    }
}

void NF_SetExBgPal(int screen, u32 layer, u32 pal)
{
    if (pal > 15)
        NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));

    if (!NF_TILEDBG_LAYERS[screen][layer].created)
    {
        char text[32];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(105, text, layer);
    }

    u32 mapsize = NF_TILEDBG[NF_TILEDBG_LAYERS[screen][layer].bgslot].mapsize;

    for (u32 pos = 0; pos < mapsize; pos += 2)
    {
        // Extract the top 8 bits of the map entry
        u32 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (pos + 1));

        // Preserve all bits but the palette and set the new palette
        u32 data = (hibyte & 0x0F) | (pal << 4);

        *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (pos + 1)) = data;
    }

    // Update the copy of the map in VRAM
    NF_UpdateVramMap(screen, layer);
}

void NF_SetTileHflip(int screen, u32 layer, u32 tile_x, u32 tile_y)
{
    u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

    // Extract the top 8 bits of the map entry
    u32 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

    // Flip the bit that controls the horizontal flip of the tile
    hibyte ^= 0x04;

    *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = hibyte;
}

void NF_SetTileVflip(int screen, u32 layer, u32 tile_x, u32 tile_y)
{
    u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

    // Extract the top 8 bits of the map entry
    u32 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

    // Flip the bit that controls the vertical flip of the tile
    hibyte ^= 0x08;

    *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = hibyte;
}

void NF_RotateTileGfx(u32 slot, u32 tile, u32 rotation)
{
    // Allocate temporary buffers
    char *character_a = malloc(64);
    if (character_a == NULL)
        NF_Error(102, NULL, 64);

    char *character_b = malloc(64);
    if (character_b == NULL)
        NF_Error(102, NULL, 64);

    // Copy tile to a temporary working buffer
    memcpy(character_a, NF_BUFFER_BGTILES[slot] + (tile * 64), 64);

    int xb, yb;

    switch (rotation)
    {
        case 1: // 90 degrees clockwise
            xb = 7;
            yb = 0;
            for (int ya = 0; ya < 8; ya++)
            {
                for (int xa = 0; xa < 8; xa++)
                {
                    int pos_a = (ya * 8) + xa;
                    int pos_b = (yb * 8) + xb;
                    character_b[pos_b] = character_a[pos_a];
                    yb++;
                    if (yb > 7)
                    {
                        yb = 0;
                        xb--;
                    }
                }
            }
            break;

        case 2: // 90 degrees counter-clockwise
            xb = 0;
            yb = 7;
            for (int ya = 0; ya < 8; ya++)
            {
                for (int xa = 0; xa < 8; xa++)
                {
                    int pos_a = (ya * 8) + xa;
                    int pos_b = (yb * 8) + xb;
                    character_b[pos_b] = character_a[pos_a];
                    yb--;
                    if (yb < 0)
                    {
                        yb = 7;
                        xb++;
                    }
                }
            }
            break;

        case 3: // 180 degrees
            xb = 7;
            yb = 7;
            for (int ya = 0; ya < 8; ya++)
            {
                for (int xa = 0; xa < 8; xa++)
                {
                    int pos_a = (ya * 8) + xa;
                    int pos_b = (yb * 8) + xb;
                    character_b[pos_b] = character_a[pos_a];
                    xb--;
                    if (xb < 0)
                    {
                        xb = 0;
                        yb--;
                    }
                }
            }
            break;

        default:
            break;
    }

    // Copy the rotated tile to its final destination
    memcpy(NF_BUFFER_BGTILES[slot] + (tile * 64), character_b, 64);

    // Free temporary buffers
    free(character_a);
    free(character_b);
}
