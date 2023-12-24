// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Collision maps functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_basic.h"
#include "nf_collision.h"

// Struct that holds information about all collision maps
NF_TYPE_CMAP_INFO NF_CMAP[NF_SLOTS_CMAP];

void NF_InitCmapBuffers(void)
{
    for (int n = 0; n < NF_SLOTS_CMAP; n++)
    {
        NF_CMAP[n].tiles = NULL;
        NF_CMAP[n].map = NULL;
        NF_CMAP[n].tiles_size = 0;
        NF_CMAP[n].map_size = 0;
        NF_CMAP[n].width = 0;
        NF_CMAP[n].height = 0;
        NF_CMAP[n].inuse = false; // Mark as unused
    }
}

void NF_ResetCmapBuffers(void)
{
    // Free all buffers
    for (int n = 0; n < NF_SLOTS_CMAP; n++)
    {
        free(NF_CMAP[n].tiles);
        free(NF_CMAP[n].map);
    }

    // Reset state of the collision maps
    NF_InitCmapBuffers();
}

void NF_LoadCollisionMap(const char *file, u32 id, u32 width, u32 height)
{
    // Verify that the slot ID is valid
    if (id >= NF_SLOTS_CMAP)
        NF_Error(106, "Collision Map", NF_SLOTS_CMAP);

    // Verify that this slot is free
    if (NF_CMAP[id].inuse)
        NF_Error(109, "Collision Map", id);

    // Free buffers if they were in use
    free(NF_CMAP[id].map);
    NF_CMAP[id].map = NULL;

    // File path
    char filename[256];

    // Load .CMP/.MAP file (tilemap)
    snprintf(filename, sizeof(filename), "%s/%s.cmp", NF_ROOTFOLDER, file);
    if (!NF_FileExists(filename))
    {
        snprintf(filename, sizeof(filename), "%s/%s.map", NF_ROOTFOLDER, file);
        if (!NF_FileExists(filename))
            NF_Error(101, filename, 0);
    }
    NF_FileLoad(filename, &NF_CMAP[id].map, &NF_CMAP[id].map_size, 0);

    // Save map size
    NF_CMAP[id].width = width;
    NF_CMAP[id].height = height;

    // Mark this slot as in use
    NF_CMAP[id].inuse = true;
}

void NF_UnloadCollisionMap(u32 id)
{
    // Verify that the slot ID is valid
    if (id >= NF_SLOTS_CMAP)
        NF_Error(106, "Collision Map", NF_SLOTS_CMAP);

    // Verify that this slot is used
    if (!NF_CMAP[id].inuse)
        NF_Error(110, "Collision Map", id);

    // Free buffer
    free(NF_CMAP[id].map);
    NF_CMAP[id].map = NULL;

    // Mark this slot as free
    NF_CMAP[id].inuse = false;
}

u32 NF_GetTile(u32 slot, s32 x, s32 y)
{
    // If the coordinate is outside of the map, return 0
    if ((x < 0) || (y < 0) || (x >= NF_CMAP[slot].width) || (y >= NF_CMAP[slot].height))
        return 0;

    // Calculate width of the map in tiles
    u32 columns = NF_CMAP[slot].width / 8;

    // Calculate tile where the pixel is located
    u32 tile_x = x / 8;
    u32 tile_y = (y / 8) + 1; // Skip first row, it's used for the tile reference

    // Calculate the address of the tile in the map
    u32 address = ((tile_y * columns) + tile_x) * 2;

    // Read tile number
    u32 lobyte = *(NF_CMAP[slot].map + address);
    u32 hibyte = *(NF_CMAP[slot].map + (address + 1));

    return (hibyte << 8) | lobyte;
}

void NF_SetTile(u32 slot, s32 x, s32 y, u32 value)
{
    // If the coordinate is outside of the map, return
    if ((x < 0) && (y < 0) && (x >= NF_CMAP[slot].width) && (y >= NF_CMAP[slot].height))
        return;

    // Calculate width of the map in tiles
    u32 columns = NF_CMAP[slot].width / 8;

    // Calculate tile where the pixel is located
    u32 tile_x = x / 8;
    u32 tile_y = (y / 8) + 1; // Skip first row, it's used for the tile reference

    // Calculate the address of the tile in the map
    s32 address = ((tile_y * columns) + tile_x) * 2; // Each tile uses 2 bytes

    // Split new tile number
    u32 hibyte = (value >> 8) & 0xff;
    u32 lobyte = value & 0xff;

    // Write the new tile number
    *(NF_CMAP[slot].map + address) = lobyte;
    *(NF_CMAP[slot].map + (address + 1)) = hibyte;
}

void NF_LoadCollisionBg(const char *file, u32 id, u32 width, u32 height)
{
    // Verify that the slot ID is valid
    if (id >= NF_SLOTS_CMAP)
        NF_Error(106, "Collision map", NF_SLOTS_CMAP);

    // Verify that this slot is free
    if (NF_CMAP[id].inuse)
        NF_Error(109, "Collision map", id);

    // Free buffers if they were in use
    free(NF_CMAP[id].tiles);
    NF_CMAP[id].tiles = NULL;
    free(NF_CMAP[id].map);
    NF_CMAP[id].map = NULL;

    // File path
    char filename[256];

    // Load .DAT/.IMG file (tileset)
    snprintf(filename, sizeof(filename), "%s/%s.dat", NF_ROOTFOLDER, file);
    if (!NF_FileExists(filename))
    {
        snprintf(filename, sizeof(filename), "%s/%s.img", NF_ROOTFOLDER, file);
        if (!NF_FileExists(filename))
            NF_Error(101, filename, 0);
    }
    NF_FileLoad(filename, &NF_CMAP[id].tiles, &NF_CMAP[id].tiles_size, 0);

    // Load .CMP/.MAP file (tilemap)
    snprintf(filename, sizeof(filename), "%s/%s.cmp", NF_ROOTFOLDER, file);
    if (!NF_FileExists(filename))
    {
        snprintf(filename, sizeof(filename), "%s/%s.map", NF_ROOTFOLDER, file);
        if (!NF_FileExists(filename))
            NF_Error(101, filename, 0);
    }
    NF_FileLoad(filename, &NF_CMAP[id].map, &NF_CMAP[id].map_size, 0);

    // Save map size
    NF_CMAP[id].width = width;
    NF_CMAP[id].height = height;

    // Mark this slot as in use
    NF_CMAP[id].inuse = true;
}

void NF_UnloadCollisionBg(u32 id)
{
    // Verify that the slot ID is valid
    if (id >= NF_SLOTS_CMAP)
        NF_Error(106, "Collision map", NF_SLOTS_CMAP);

    // Verify that this slot is used
    if (!NF_CMAP[id].inuse)
        NF_Error(110, "Collision map", id);

    // Free the buffers
    free(NF_CMAP[id].tiles);
    NF_CMAP[id].tiles = NULL;
    free(NF_CMAP[id].map);
    NF_CMAP[id].map = NULL;

    // Mark this map as unused
    NF_CMAP[id].inuse = false;
}

u8 NF_GetPoint(u32 slot, s32 x, s32 y)
{
    // If the coordinate is outside of the map, return 0
    if ((x < 0) || (y < 0) || (x >= NF_CMAP[slot].width) || (y >= NF_CMAP[slot].height))
        return 0;

    // Calculate width of the map in tiles
    u32 columns = NF_CMAP[slot].width / 8;

    // Calculate tile where the pixel is located
    u32 tile_x = x / 8;
    u32 tile_y = (y / 8) + 1; // Skip first row, it's used for the tile reference

    // Calculate pixel coordinates inside the tile
    u32 pixel_x = x & 7;
    u32 pixel_y = y & 7;

    // Calculate the address of the tile in the map
    s32 address = ((tile_y * columns) + tile_x) * 2; // Each tile uses 2 bytes

    // Read tile number
    u32 lobyte = *(NF_CMAP[slot].map + address);
    u32 hibyte = *(NF_CMAP[slot].map + (address + 1));
    u32 tile = (hibyte << 8) | lobyte;

    // Read value of the pixel inside the tile
    address = (tile * 64) + (pixel_y * 8) + pixel_x;
    lobyte = *(NF_CMAP[slot].tiles + address);

    return lobyte;
}
