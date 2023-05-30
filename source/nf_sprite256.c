// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 256 color sprites functions
// http://www.nightfoxandco.com/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_sprite256.h"

// Buffers to store the sprite graphics and palettes
char *NF_BUFFER_SPR256GFX[NF_SLOTS_SPR256GFX];
char *NF_BUFFER_SPR256PAL[NF_SLOTS_SPR256PAL];

// Information structures for sprite graphics and palettes
NF_TYPE_SPR256GFX_INFO NF_SPR256GFX[NF_SLOTS_SPR256GFX];
NF_TYPE_SPR256PAL_INFO NF_SPR256PAL[NF_SLOTS_SPR256PAL];

// Structures for sprite graphics and palettes stored in VRAM
NF_TYPE_SPR256VRAM_INFO NF_SPR256VRAM[2][128];
NF_TYPE_SPRPALSLOT_INFO NF_SPRPALSLOT[2][16];

// Structure with OAM information of sprites
NF_TYPE_SPRITEOAM_INFO NF_SPRITEOAM[2][128];

// Main VRAM control structure for sprites
NF_TYPE_SPRVRAM_INFO NF_SPRVRAM[2];

void NF_InitSpriteBuffers(void)
{
    // Initialize graphics buffers
    for (int n = 0; n < NF_SLOTS_SPR256GFX; n++)
    {
        NF_BUFFER_SPR256GFX[n] = NULL;
        NF_SPR256GFX[n].size = 0;
        NF_SPR256GFX[n].width = 0;
        NF_SPR256GFX[n].height = 0;
        NF_SPR256GFX[n].available = true; // Mark as available
    }

    // Initialize palette buffers
    for (int n = 0; n < NF_SLOTS_SPR256PAL; n++)
    {
        NF_BUFFER_SPR256PAL[n] = NULL;
        NF_SPR256PAL[n].size = 0;
        NF_SPR256PAL[n].available = true; // Mark as available
    }
}

void NF_ResetSpriteBuffers(void)
{
    // Free graphics and palette buffers
    for (int n = 0; n < NF_SLOTS_SPR256GFX; n++)
        free(NF_BUFFER_SPR256GFX[n]);

    for (int n = 0; n < NF_SLOTS_SPR256PAL; n++)
        free(NF_BUFFER_SPR256PAL[n]);

    // Reset sprite system
    NF_InitSpriteBuffers();
}

void NF_InitSpriteSys(int screen, ...)
{
    va_list options;
    va_start(options, screen);
    u8 mode = va_arg(options, int);
    va_end(options);

    // Initialize sprite allocator and OAM information
    for (int n = 0; n < 128; n++)
    {
        // Graphics in VRAM
        NF_SPR256VRAM[screen][n].size = 0;
        NF_SPR256VRAM[screen][n].width = 0;
        NF_SPR256VRAM[screen][n].height = 0;
        NF_SPR256VRAM[screen][n].address = 0;
        NF_SPR256VRAM[screen][n].ramid = 0;
        NF_SPR256VRAM[screen][n].framesize = 0;
        NF_SPR256VRAM[screen][n].lastframe = 0;
        NF_SPR256VRAM[screen][n].keepframes = false;
        NF_SPR256VRAM[screen][n].inuse = false; // Mark as not in use

        // OAM
        NF_SPRITEOAM[screen][n].index = n;
        NF_SPRITEOAM[screen][n].x = 0;
        NF_SPRITEOAM[screen][n].y = 0;
        NF_SPRITEOAM[screen][n].layer = 0;
        NF_SPRITEOAM[screen][n].pal = 0;
        NF_SPRITEOAM[screen][n].size = SpriteSize_8x8;
        NF_SPRITEOAM[screen][n].color = SpriteColorFormat_256Color;
        NF_SPRITEOAM[screen][n].gfx = NULL;
        NF_SPRITEOAM[screen][n].rot = -1;
        NF_SPRITEOAM[screen][n].doublesize = false;
        NF_SPRITEOAM[screen][n].hide = true;
        NF_SPRITEOAM[screen][n].hflip = false;
        NF_SPRITEOAM[screen][n].vflip = false;
        NF_SPRITEOAM[screen][n].mosaic = false;
        NF_SPRITEOAM[screen][n].gfxid = 0;
        NF_SPRITEOAM[screen][n].frame = 0;
        NF_SPRITEOAM[screen][n].framesize = 0;
        NF_SPRITEOAM[screen][n].lastframe = 0;
        NF_SPRITEOAM[screen][n].created = false; // Mark sprite as not created
    }

    // Initialize VRAM allocator
    if (mode == 128)
        NF_SPRVRAM[screen].max = 131072;
    else
        NF_SPRVRAM[screen].max = 65536;

    NF_SPRVRAM[screen].free = NF_SPRVRAM[screen].max; // All memory is available
    NF_SPRVRAM[screen].last = 0;
    NF_SPRVRAM[screen].deleted = 0;
    NF_SPRVRAM[screen].fragmented = 0;
    NF_SPRVRAM[screen].inarow = NF_SPRVRAM[screen].max; // All memory is contiguous
    for (int n = 0; n < 128; n++)
    {
        NF_SPRVRAM[screen].pos[n] = 0;
        NF_SPRVRAM[screen].size[n] = 0;
    }

    // Initialize palette information
    for (int n = 0; n < 16; n++)
    {
        NF_SPRPALSLOT[screen][n].inuse = false;
        NF_SPRPALSLOT[screen][n].ramslot = 0;
    }

    // Setup 2D engine and VRAM banks of the specified screen
    if (screen == 0)
    {
        REG_DISPCNT |= DISPLAY_SPR_ACTIVE; // Enable sprites in the top screen
        vramSetBankB(VRAM_B_LCD); // Enable CPU writes to VRAM_B
        memset((void *)0x06400000, 0, 131072); // Clear VRAM_B
        vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000); // VRAM_B is used for sprites (128 KB)
        NF_SPRVRAM[screen].next = 0x06400000;
        vramSetBankF(VRAM_F_LCD); // VRAM_F used for sprite ext. palettes (8 / 16 KB)
        memset((void *)0x06890000, 0, 8192); // Clear VRAM_F

        // Enable OAM with 128 or 64 mapping and extended palettes enabled
        if (mode == 128)
            oamInit(&oamMain, SpriteMapping_1D_128, true);
        else
            oamInit(&oamMain, SpriteMapping_1D_64, true);
    }
    else
    {
        REG_DISPCNT_SUB |= DISPLAY_SPR_ACTIVE; // Enable sprites in the bottom screen
        vramSetBankD(VRAM_D_LCD); // Enable CPU writes to VRAM_D
        memset((void *)0x06600000, 0, 131072); // Clear VRAM_D
        vramSetBankD(VRAM_D_SUB_SPRITE);
        NF_SPRVRAM[screen].next = 0x06600000;
        vramSetBankI(VRAM_I_LCD); // VRAM_I used for sprite ext. palettes (8 / 16 KB)
        memset((void *)0x068A0000, 0, 8192); // Clear VRAM_I

        // Enable OAM with 128 or 64 mapping and extended palettes enabled
        if (mode == 128)
            oamInit(&oamSub, SpriteMapping_1D_128, true);
        else
            oamInit(&oamSub, SpriteMapping_1D_64, true);
    }
}

void NF_LoadSpriteGfx(const char *file, u32 id, u32 width, u32 height)
{
    if (id >= NF_SLOTS_SPR256GFX)
        NF_Error(106, "Sprite graphics", NF_SLOTS_SPR256GFX);

    if (!NF_SPR256GFX[id].available)
        NF_Error(109, "Sprite graphics", id);

    // Free the buffer if it was in use
    free(NF_BUFFER_SPR256GFX[id]);
    NF_BUFFER_SPR256GFX[id] = NULL;

    // File path
    char filename[256];

    // Load .IMG file
    snprintf(filename, sizeof(filename), "%s/%s.img", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_SPR256GFX[id], &NF_SPR256GFX[id].size, 0);

    // Save information about the graphics
    NF_SPR256GFX[id].width = width;
    NF_SPR256GFX[id].height = height;

    NF_SPR256GFX[id].available = false; // Mark ID as being in use
}

void NF_UnloadSpriteGfx(u32 id)
{
    if (id >= NF_SLOTS_SPR256GFX)
        NF_Error(106, "Sprite graphics", NF_SLOTS_SPR256GFX);

    if (NF_SPR256GFX[id].available)
        NF_Error(110, "Sprite graphics", id);

    // Free buffer
    free(NF_BUFFER_SPR256GFX[id]);

    // Reset variables
    NF_BUFFER_SPR256GFX[id] = NULL;
    NF_SPR256GFX[id].size = 0;
    NF_SPR256GFX[id].width = 0;
    NF_SPR256GFX[id].height = 0;
    NF_SPR256GFX[id].available = true; // Mark as available
}

void NF_LoadSpritePal(const char *file, u32 id)
{
    if (id >= NF_SLOTS_SPR256PAL)
        NF_Error(106, "Sprite palette", NF_SLOTS_SPR256PAL);

    if (!NF_SPR256PAL[id].available)
        NF_Error(109, "Sprite palette", id);

    // Free buffer if it was used already
    free(NF_BUFFER_SPR256PAL[id]);
    NF_BUFFER_SPR256PAL[id] = NULL;

    // FIle path
    char filename[256];

    // Load .PAL file, and allocate space for at least 256 colors
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_SPR256PAL[id], &NF_SPR256PAL[id].size, 256 * 2);

    // Mark it as being in use
    NF_SPR256PAL[id].available = false;
}

void NF_UnloadSpritePal(u32 id)
{
    if (id >= NF_SLOTS_SPR256PAL)
        NF_Error(106, "Sprite palette", NF_SLOTS_SPR256PAL);

    if (NF_SPR256PAL[id].available)
        NF_Error(110, "Sprite palette", id);

    // Free buffer
    free(NF_BUFFER_SPR256PAL[id]);

    // Reset variables
    NF_BUFFER_SPR256PAL[id] = NULL;
    NF_SPR256PAL[id].size = 0;
    NF_SPR256PAL[id].available = true; // Mark slot as free
}

void NF_VramSpriteGfx(int screen, u32 ram, u32 vram, bool keepframes)
{
    if (ram >= NF_SLOTS_SPR256GFX)
        NF_Error(106, "Sprite graphics", (NF_SLOTS_SPR256GFX - 1));

    if (NF_SPR256GFX[ram].available)
        NF_Error(110, "Sprite graphics", ram);

    if (vram > 127)
        NF_Error(106, "VRAM graphics", 127);

    if (NF_SPR256VRAM[screen][vram].inuse)
        NF_Error(109, "VRAM", vram);

    s16 id = 255; // This will hold the ID of a free slot

    // Calculate the size of one frame. One time is 64 bytes (8 * 8)
    u32 width = NF_SPR256GFX[ram].width / 8;
    u32 height = NF_SPR256GFX[ram].height / 8;
    NF_SPR256VRAM[screen][vram].framesize = (width * height) * 64;

    // Calculate the last frame of the animation
    NF_SPR256VRAM[screen][vram].lastframe =
            (NF_SPR256GFX[ram].size / NF_SPR256VRAM[screen][vram].framesize) - 1;
    NF_SPR256VRAM[screen][vram].inuse = true; // Mark this slot as being in use

    // Calculate the size of the graphics to copy depending on whether all
    // frames need to be copied or not.
    u32 gfxsize;
    if (keepframes) // Copy the first frame only
        gfxsize = NF_SPR256VRAM[screen][vram].framesize;
    else // Copy all frames
        gfxsize = NF_SPR256GFX[ram].size;

    // Update the available VRAM
    NF_SPRVRAM[screen].free -= gfxsize;
    if (NF_SPRVRAM[screen].free < 0) // If there isn't enough VRAM, fail
        NF_Error(113, "Sprites", gfxsize);

    // Try to reuse any deleted block
    if (NF_SPRVRAM[screen].deleted > 0)
    {
        // First, try to find any block with the same size
        for (int n = 0; n < NF_SPRVRAM[screen].deleted; n++)
        {
            if (NF_SPRVRAM[screen].size[n] == gfxsize)
            {
                id = n;
                break;
            }
        }

        // If not, try to find any block that is big enough, fragmenting it
        if (id == 255)
        {
            for (int n = 0; n < NF_SPRVRAM[screen].deleted; n++)
            {
                if (NF_SPRVRAM[screen].size[n] > gfxsize)
                {
                    id = n;
                    break;
                }
            }
        }
    }

    if (id != 255)
    {
        // We have managed to reuse a deleted block, determine if we need to
        // reorganize the VRAM allocation structures.

        bool organize = true; // Do we have to reorganize the free blocks array?

        // Copy graphics to VRAM
        NF_DmaMemCopy((void *)NF_SPRVRAM[screen].pos[id], NF_BUFFER_SPR256GFX[ram], gfxsize);
        // Save the destination address
        NF_SPR256VRAM[screen][vram].address = NF_SPRVRAM[screen].pos[id];

        // If not all the size has been used, split this block into used and free
        if (gfxsize < NF_SPRVRAM[screen].size[id])
        {
            // Calculate the new free space
            u32 size = NF_SPRVRAM[screen].size[id] - gfxsize;

            // Update data
            NF_SPRVRAM[screen].pos[id] += gfxsize; // New free space address
            NF_SPRVRAM[screen].size[id] = size;    // New free space size
            NF_SPRVRAM[screen].fragmented -= gfxsize; // Reduce the number or fragmented bytes
            organize = false; // We don't need to reorganize anything after this
        }
        else
        {
            // If all the size has been used, simply reduce the counter of
            // fragmented bytes.
            NF_SPRVRAM[screen].fragmented -= NF_SPRVRAM[screen].size[id];
        }

        // Do we have to reorganize the array of free blocks?
        if (organize)
        {
            int last_reuse = NF_SPRVRAM[screen].deleted - 1;

            // If there are more than one deleted blocks and this isn't the last
            // position.
            if ((last_reuse > 0) && (id != last_reuse))
            {
                // Copy the values of the last position in this one
                NF_SPRVRAM[screen].pos[id] = NF_SPRVRAM[screen].pos[last_reuse];
                NF_SPRVRAM[screen].size[id] = NF_SPRVRAM[screen].size[last_reuse];
            }
            NF_SPRVRAM[screen].deleted--; // Update counter of free blocks
        }

    }
    else
    {
        // We couldn't reuse any deleted block, add the new graphics to the end
        // of the used VRAM.

        // Update the contiguous available VRAM (the last block at the end)
        NF_SPRVRAM[screen].inarow -= gfxsize;

        // If there isn't enough VRAM, fail
        if (NF_SPRVRAM[screen].inarow < 0)
            NF_Error(113, "Sprites", gfxsize);

        // Copy graphics to VRAM
        NF_DmaMemCopy((void *)NF_SPRVRAM[screen].next, NF_BUFFER_SPR256GFX[ram], gfxsize);

        // Copy location and size in VRAM
        NF_SPR256VRAM[screen][vram].address = NF_SPRVRAM[screen].next;
        NF_SPRVRAM[screen].last = NF_SPRVRAM[screen].next;

        // Calculate the next available address
        NF_SPRVRAM[screen].next += gfxsize;
    }

    // Save graphics information
    NF_SPR256VRAM[screen][vram].size = gfxsize;
    NF_SPR256VRAM[screen][vram].width = NF_SPR256GFX[ram].width;
    NF_SPR256VRAM[screen][vram].height = NF_SPR256GFX[ram].height;
    NF_SPR256VRAM[screen][vram].ramid = ram; // Source slot in RAM
    NF_SPR256VRAM[screen][vram].keepframes = keepframes;
}

void NF_FreeSpriteGfx(int screen, u32 id)
{
    if (!NF_SPR256VRAM[screen][id].inuse)
        NF_Error(110, "Sprite graphics", id);

    // Clear graphics from VRAM
    memset((void *)NF_SPR256VRAM[screen][id].address, 0, NF_SPR256VRAM[screen][id].size);

    // Update free VRAM
    NF_SPRVRAM[screen].free += NF_SPR256VRAM[screen][id].size;

    // Save address and size of the deleted block to be reused
    NF_SPRVRAM[screen].pos[NF_SPRVRAM[screen].deleted] = NF_SPR256VRAM[screen][id].address;
    NF_SPRVRAM[screen].size[NF_SPRVRAM[screen].deleted] = NF_SPR256VRAM[screen][id].size;

    // Increment counter of deleted blocks
    NF_SPRVRAM[screen].deleted++;

    // Increment counter of fragmented memory
    NF_SPRVRAM[screen].fragmented += NF_SPR256VRAM[screen][id].size;

    // Reset graphics state
    NF_SPR256VRAM[screen][id].size = 0;
    NF_SPR256VRAM[screen][id].width = 0;
    NF_SPR256VRAM[screen][id].height = 0;
    NF_SPR256VRAM[screen][id].address = 0;
    NF_SPR256VRAM[screen][id].framesize = 0;
    NF_SPR256VRAM[screen][id].lastframe = 0;
    NF_SPR256VRAM[screen][id].inuse = false;

    // If the memory is too fragmented, defragment it
    if (NF_SPRVRAM[screen].fragmented >= (NF_SPRVRAM[screen].inarow / 2))
        NF_VramSpriteGfxDefrag(screen);
}

void NF_VramSpriteGfxDefrag(int screen)
{
    // Allocate temporary buffer with all the currently used VRAM
    u32 used_vram = (NF_SPRVRAM[screen].max - NF_SPRVRAM[screen].free) + 1;
    char *buffer = malloc(used_vram);
    if (buffer == NULL) // Not enough RAM
        NF_Error(102, NULL, used_vram);

    char *address[128];     // RAM address of each sprite
    u32 size[128];          // Size of buffer
    u32 ram = 0;            // Pointer to the current RAM address
    u32 frame_address = 0;  // Address of the current frame

    // Copy data from VRAM to the temporary buffer in RAM
    for (int n = 0; n < 128; n++)
    {
        // Copy all used graphics to the temporary buffer
        if (NF_SPR256VRAM[screen][n].inuse)
        {
            address[n] = buffer + ram;
            size[n] = NF_SPR256VRAM[screen][n].size;
            NF_DmaMemCopy(address[n], (void *)NF_SPR256VRAM[screen][n].address, size[n]);
            ram += size[n];
        }
    }

    // Reset VRAM allocation information
    NF_SPRVRAM[screen].free = NF_SPRVRAM[screen].max;
    NF_SPRVRAM[screen].last = 0;
    NF_SPRVRAM[screen].deleted = 0;
    NF_SPRVRAM[screen].fragmented = 0;
    NF_SPRVRAM[screen].inarow = NF_SPRVRAM[screen].max;

    for (int n = 0; n < 128; n++)
    {
        NF_SPRVRAM[screen].pos[n] = 0;
        NF_SPRVRAM[screen].size[n] = 0;
    }

    if (screen == 0)
        NF_SPRVRAM[screen].next = 0x06400000;
    else
        NF_SPRVRAM[screen].next = 0x06600000;

    // Now copy all data from the temporary buffer to VRAM
    for (int n = 0; n < 128; n++)
    {
        if (NF_SPR256VRAM[screen][n].inuse)
        {
            NF_DmaMemCopy((void*)NF_SPRVRAM[screen].next, address[n], size[n]);
            NF_SPR256VRAM[screen][n].address = NF_SPRVRAM[screen].next;

            NF_SPRVRAM[screen].free -= size[n];
            NF_SPRVRAM[screen].inarow -= size[n];
            NF_SPRVRAM[screen].last = NF_SPRVRAM[screen].next;
            NF_SPRVRAM[screen].next += size[n];
        }
    }

    // Link the new graphics addresses to the sprites
    for (int n = 0; n < 128; n++)
    {
        if (NF_SPRITEOAM[screen][n].created)
        {
            if (NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][n].gfxid].keepframes)
            {
                // If keepframes is enabled, only assign the base address of the
                // graphics
                NF_SPRITEOAM[screen][n].gfx =
                    (u32 *)NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][n].gfxid].address;
            }
            else
            {
                // If keepframes is disabled calculate the offset of the
                // graphics
                frame_address = NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][n].gfxid].address
                    + (NF_SPRITEOAM[screen][n].framesize * NF_SPRITEOAM[screen][n].frame);
                NF_SPRITEOAM[screen][n].gfx = (u32 *)frame_address;
            }
        }
    }

    // Free temporary buffer
    free(buffer);
}

void NF_VramSpritePal(int screen, u32 id, u32 slot)
{
    if (id >= NF_SLOTS_SPR256PAL)
        NF_Error(106, "Sprite palette", NF_SLOTS_SPR256PAL);

    if (NF_SPR256PAL[id].available)
        NF_Error(110, "Sprite palette", id);

    if (slot > 15)
        NF_Error(106, "Sprite palette slot", 15);

    // Copy palette to VRAM
    if (screen == 0)
    {
        u32 address = 0x06890000 + (slot * 256 * 2);
        vramSetBankF(VRAM_F_LCD); // Enable CPU writes to VRAM_F
        NF_DmaMemCopy((void *)address, NF_BUFFER_SPR256PAL[id], NF_SPR256PAL[id].size);
        vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);
    }
    else
    {
        u32 address = 0x068A0000 + (slot * 256 * 2);
        vramSetBankI(VRAM_I_LCD); // Enable CPU writes to VRAM_I
        NF_DmaMemCopy((void *)address, NF_BUFFER_SPR256PAL[id], NF_SPR256PAL[id].size);
        vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    }

    NF_SPRPALSLOT[screen][slot].inuse = true; // Mark slot as in use
    NF_SPRPALSLOT[screen][slot].ramslot = id; // Save slot where the palette is stored
}

void NF_CreateSprite(int screen, u32 id, u32 gfx, u32 pal, s32 x, s32 y)
{
    if (id > 127)
        NF_Error(106, "Sprite ID", 127);

    if (gfx > 127)
        NF_Error(106, "Sprite graphics", 127);

    if (!NF_SPR256VRAM[screen][gfx].inuse)
        NF_Error(111, "Sprite graphics", gfx);

    if (pal > 15)
        NF_Error(106, "Sprite palette slot", 15);

    if (!NF_SPRPALSLOT[screen][pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    NF_SPRITEOAM[screen][id].index = id;
    NF_SPRITEOAM[screen][id].gfx = (u32 *)NF_SPR256VRAM[screen][gfx].address; // VRAM address
    NF_SPRITEOAM[screen][id].pal = pal;
    NF_SPRITEOAM[screen][id].x = x;
    NF_SPRITEOAM[screen][id].y = y;
    NF_SPRITEOAM[screen][id].color = SpriteColorFormat_256Color;
    NF_SPRITEOAM[screen][id].hide = false; // Show sprite
    NF_SPRITEOAM[screen][id].gfxid = gfx; // Graphics index
    NF_SPRITEOAM[screen][id].created = true; // Mark sprite as created

    if ((NF_SPR256VRAM[screen][gfx].width == 8) && (NF_SPR256VRAM[screen][gfx].height == 8))
    {
        if (NF_SPRVRAM[screen].max != 131072) // In mode 1D_128 this size is invalid
            NF_SPRITEOAM[screen][id].size = SpriteSize_8x8;
        else
            NF_Error(120, NULL, id);
    }
    else
    {
        struct
        {
            int x, y;
            SpriteSize size;
        }
        size_array[] =
        {
            { 16, 16, SpriteSize_16x16 },
            { 32, 32, SpriteSize_32x32 },
            { 64, 64, SpriteSize_64x64 },
            { 16, 8, SpriteSize_16x8 },
            { 32, 8, SpriteSize_32x8 },
            { 32, 16, SpriteSize_32x16 },
            { 64, 32, SpriteSize_64x32 },
            { 8, 16, SpriteSize_8x16 },
            { 8, 32, SpriteSize_8x32 },
            { 16, 32, SpriteSize_16x32 },
            { 32, 64, SpriteSize_16x32 },
            { 0, 0, 0 },
        };

        int i = 0;
        while (1)
        {
            if (size_array[i].x == 0)
                NF_Error(106, "Sprite size", 0);

            if ((NF_SPR256VRAM[screen][gfx].width == size_array[i].x) &&
                (NF_SPR256VRAM[screen][gfx].height == size_array[i].y))
            {
                NF_SPRITEOAM[screen][id].size = size_array[i].size;
                break;
            }

            i++;
        }
    }

    NF_SPRITEOAM[screen][id].lastframe = NF_SPR256VRAM[screen][gfx].lastframe;
    NF_SPRITEOAM[screen][id].framesize = NF_SPR256VRAM[screen][gfx].framesize;

    NF_SPRITEOAM[screen][id].frame = 0; // Start at frame 0
}

void NF_DeleteSprite(int screen, u32 id)
{
    if (id > 127)
        NF_Error(106, "Sprite ID", 127);

    // Verify that the sprite is created
    if (!NF_SPRITEOAM[screen][id].created)
    {
        char text[4];
        snprintf(text, sizeof(text), "%d", screen);
        NF_Error(112, text, id);
    }

    // Reset sprite state
    NF_SPRITEOAM[screen][id].index = id;
    NF_SPRITEOAM[screen][id].x = 0;
    NF_SPRITEOAM[screen][id].y = 0;
    NF_SPRITEOAM[screen][id].layer = 0;
    NF_SPRITEOAM[screen][id].pal = 0;
    NF_SPRITEOAM[screen][id].size = SpriteSize_8x8;
    NF_SPRITEOAM[screen][id].color = SpriteColorFormat_256Color;
    NF_SPRITEOAM[screen][id].gfx = NULL;
    NF_SPRITEOAM[screen][id].rot = -1;
    NF_SPRITEOAM[screen][id].doublesize = false;
    NF_SPRITEOAM[screen][id].hide = true;
    NF_SPRITEOAM[screen][id].hflip = false;
    NF_SPRITEOAM[screen][id].vflip = false;
    NF_SPRITEOAM[screen][id].mosaic = false;
    NF_SPRITEOAM[screen][id].gfxid = 0;
    NF_SPRITEOAM[screen][id].frame = 0;
    NF_SPRITEOAM[screen][id].framesize = 0;
    NF_SPRITEOAM[screen][id].lastframe = 0;
    NF_SPRITEOAM[screen][id].created = false; // Mark sprite as not created
}

void NF_SpriteOamSet(int screen)
{
    if (screen == 0)
    {
        for (int n = 0; n < 128; n++)
        {
            oamSet(&oamMain,
                NF_SPRITEOAM[screen][n].index,
                NF_SPRITEOAM[screen][n].x,
                NF_SPRITEOAM[screen][n].y,
                NF_SPRITEOAM[screen][n].layer,
                NF_SPRITEOAM[screen][n].pal,
                NF_SPRITEOAM[screen][n].size,
                NF_SPRITEOAM[screen][n].color,
                NF_SPRITEOAM[screen][n].gfx,
                NF_SPRITEOAM[screen][n].rot,
                NF_SPRITEOAM[screen][n].doublesize,
                NF_SPRITEOAM[screen][n].hide,
                NF_SPRITEOAM[screen][n].hflip,
                NF_SPRITEOAM[screen][n].vflip,
                NF_SPRITEOAM[screen][n].mosaic);
        }
    }
    else
    {
        for (int n = 0; n < 128; n++)
        {
            oamSet(&oamSub,
                NF_SPRITEOAM[screen][n].index,
                NF_SPRITEOAM[screen][n].x,
                NF_SPRITEOAM[screen][n].y,
                NF_SPRITEOAM[screen][n].layer,
                NF_SPRITEOAM[screen][n].pal,
                NF_SPRITEOAM[screen][n].size,
                NF_SPRITEOAM[screen][n].color,
                NF_SPRITEOAM[screen][n].gfx,
                NF_SPRITEOAM[screen][n].rot,
                NF_SPRITEOAM[screen][n].doublesize,
                NF_SPRITEOAM[screen][n].hide,
                NF_SPRITEOAM[screen][n].hflip,
                NF_SPRITEOAM[screen][n].vflip,
                NF_SPRITEOAM[screen][n].mosaic);
        }
    }
}

void NF_SpriteSetPalColor(int screen, u32 pal, u32 number, u32 r, u32 g, u32 b)
{
    if (!NF_SPRPALSLOT[screen][pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    // Pack RGB value
    u32 rgb = r | (g << 5) | (b << 10);

    // Modify palette
    if (screen == 0)
    {
        u32 address = 0x06890000 + (pal * 256 * 2) + (number * 2);
        vramSetBankF(VRAM_F_LCD); // Enable CPU writes to VRAM_F
        *((u16 *)address) = rgb;
        vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);
    }
    else
    {
        u32 address = 0x068A0000 + (pal * 256 * 2) + (number * 2);
        vramSetBankI(VRAM_I_LCD); // Enable CPU writes to VRAM_I
        *((u16 *)address) = rgb;
        vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    }
}

void NF_SpriteEditPalColor(int screen, u32 pal, u32 number, u32 r, u32 g, u32 b)
{
    if (!NF_SPRPALSLOT[screen][pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    // Pack RGB value
    u32 rgb = r | (g << 5) | (b << 10);

    u32 hibyte = (rgb >> 8) & 0xff;
    u32 lobyte = rgb & 0xff;

    *(NF_BUFFER_SPR256PAL[NF_SPRPALSLOT[screen][pal].ramslot] + (number * 2)) = lobyte;
    *(NF_BUFFER_SPR256PAL[NF_SPRPALSLOT[screen][pal].ramslot] + ((number * 2) + 1)) = hibyte;
}

void NF_SpriteUpdatePalette(int screen, u32 pal)
{
    if (!NF_SPRPALSLOT[screen][pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    // Get slot where the palette is stored in RAM
    u32 slot = NF_SPRPALSLOT[screen][pal].ramslot;

    // Update palette in VRAM
    if (screen == 0)
    {
        u32 address = 0x06890000 + (pal * 256 * 2);
        vramSetBankF(VRAM_F_LCD); // Enable CPU writes to VRAM_F
        NF_DmaMemCopy((void *)address, NF_BUFFER_SPR256PAL[slot], NF_SPR256PAL[slot].size);
        vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);
    }
    else
    {
        u32 address = 0x068A0000 + (pal * 256 * 2);
        vramSetBankI(VRAM_I_LCD); // Enable CPU writes to VRAM_I
        NF_DmaMemCopy((void *)address, NF_BUFFER_SPR256PAL[slot], NF_SPR256PAL[slot].size);
        vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    }
}

void NF_SpriteGetPalColor(int screen, u32 pal, u32 number, u8 *r, u8 *g, u8 *b)
{
    if (!NF_SPRPALSLOT[screen][pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    u32 ramslot = NF_SPRPALSLOT[screen][pal].ramslot;

    u32 lobyte = *(NF_BUFFER_SPR256PAL[ramslot] + (number * 2));
    u32 hibyte = *(NF_BUFFER_SPR256PAL[ramslot] + ((number * 2) + 1));

    // Get RGB value
    u32 rgb = (hibyte << 8) | lobyte;

    // Split components
    *r = rgb & 0x1F;
    *g = (rgb >> 5) & 0x1F;
    *b = (rgb >> 10) & 0x1F;
}
