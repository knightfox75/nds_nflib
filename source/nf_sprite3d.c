// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 3D sprite functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_3d.h"
#include "nf_basic.h"
#include "nf_sprite256.h"
#include "nf_sprite3d.h"

// 3D sprite control structs
NF_TYPE_3DSPRITE_INFO NF_3DSPRITE[NF_3DSPRITES];

// Structs that handle textures in VRAM
NF_TYPE_TEX256VRAM_INFO NF_TEX256VRAM[NF_3DSPRITES];

// Structs that handle palettes in VRAM
NF_TYPE_3DSPRPALSLOT_INFO NF_TEXPALSLOT[32];

// Texture VRAM allocator information
NF_TYPE_TEXVRAM_INFO NF_TEXVRAM;

// 3D sprite handler
NF_TYPE_CREATED_3DSPRITE_INFO NF_CREATED_3DSPRITE;

void NF_Init3dSpriteSys(void)
{
    // Reset all sprite control structures
    for (int n = 0; n < NF_3DSPRITES; n++)
    {
        NF_3DSPRITE[n].inuse = false; // Mark as unused

        NF_TEX256VRAM[n].inuse = false; // Mark as unused

        NF_CREATED_3DSPRITE.id[n] = 0;
        NF_CREATED_3DSPRITE.bck[n] = 0;
    }

    // Initialize the VRAM information struct
    NF_TEXVRAM.free = 131072;       // Available VRAM (128 KB)
    NF_TEXVRAM.last = 0;            // Last used position
    NF_TEXVRAM.deleted = 0;         // No graphics deleted
    NF_TEXVRAM.fragmented = 0;      // Fragmented VRAM
    NF_TEXVRAM.inarow = 131072;     // Contiguous VRAM

    for (int n = 0; n < NF_3DSPRITES; n++)
    {
        NF_TEXVRAM.pos[n] = 0;
        NF_TEXVRAM.size[n] = 0;
    }

    NF_TEXVRAM.next = 0x06820000; // Next VRAM address to be used for allocations

    // Initialize palette slots
    for (int n = 0; n < 32; n++)
    {
        NF_TEXPALSLOT[n].inuse = false;
        NF_TEXPALSLOT[n].ramslot = 0;
    }

    // Reset the number of created sprites
    NF_CREATED_3DSPRITE.total = 0;

    // Clear VRAM_B and VRAM_F, used for textures and palettes
    vramSetBankB(VRAM_B_LCD);
    memset((void *)0x06820000, 0, 131072);
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);

    vramSetBankF(VRAM_F_LCD);
    memset((void *)0x06890000, 0, 16384);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    NF_InitOpenGL();
}

void NF_Vram3dSpriteGfx(u32 ram, u32 vram, bool keepframes)
{
    if (ram >= NF_SLOTS_SPR256GFX)
        NF_Error(106, "Sprite GFX", NF_SLOTS_SPR256GFX - 1);

    if (NF_SPR256GFX[ram].available)
        NF_Error(110, "Sprite GFX", ram);

    if (vram >= NF_3DSPRITES)
        NF_Error(106, "VRAM GFX", NF_3DSPRITES - 1);

    if (NF_TEX256VRAM[vram].inuse)
        NF_Error(109, "VRAM", vram);

    // Check that the texture size is valid
    if ((NF_GetTextureSize(NF_SPR256GFX[ram].width) == 255) ||
        (NF_GetTextureSize(NF_SPR256GFX[ram].height) == 255))
    {
        NF_Error(119, NULL, ram);
    }

    // Let the CPU access VRAM_B
    vramSetBankB(VRAM_B_LCD);

    // Calculate the size of one frame
    u32 width = NF_SPR256GFX[ram].width / 8;
    u32 height = NF_SPR256GFX[ram].height / 8;
    NF_TEX256VRAM[vram].framesize = (width * height) * 64;

    // Calculate the last frame of the animation
    NF_TEX256VRAM[vram].lastframe =
                (NF_SPR256GFX[ram].size / NF_TEX256VRAM[vram].framesize) - 1;

    NF_TEX256VRAM[vram].inuse = true; // Mark this slot as used

    // Calculate the size of the texture to copy to VRAM depending on whether
    // frames need to be copied to VRAM or kept in RAM
    u32 gfxsize;
    if (keepframes) // Keep frames in RAM, copy only the first one to VRAM
        gfxsize = NF_TEX256VRAM[vram].framesize;
    else // Copy all frames to VRAM
        gfxsize = NF_SPR256GFX[ram].size;

    NF_TEXVRAM.free -= gfxsize;

    // Fail if there isn't enough free VRAM
    if (NF_TEXVRAM.free < 0)
        NF_Error(113, "Sprites", gfxsize);

    // Try to reuse any deleted block
    int id = 255;
    if (NF_TEXVRAM.deleted > 0)
    {
        // First, try to find a block with the same size
        for (int n = 0; n < NF_TEXVRAM.deleted; n++)
        {
            if (NF_TEXVRAM.size[n] == gfxsize)
            {
                id = n;
                break;
            }
        }

        // If not, look for a block with enough space (fragmenting it)
        if (id != 255)
        {
            for (int n = 0; n < NF_TEXVRAM.deleted; n++)
            {
                if (NF_TEXVRAM.size[n] > gfxsize)
                {
                    id = n;
                    break;
                }
            }
        }
    }

    // If we have found a block to reuse
    if (id != 255)
    {
        bool organize = true; // Do we need to reorganize the array of free blocks?

        // Copy texture to VRAM
        NF_DmaMemCopy((void *)NF_TEXVRAM.pos[id], NF_BUFFER_SPR256GFX[ram], gfxsize);

        // Save the location of the texture
        NF_TEX256VRAM[vram].address = NF_TEXVRAM.pos[id];

        // If not all the free space has been used, register a new block
        if (gfxsize < NF_TEXVRAM.size[id])
        {
            // Calculate remaining free space in the block
            u32 size = NF_TEXVRAM.size[id] - gfxsize;

            // Update block information
            NF_TEXVRAM.pos[id] += gfxsize;
            NF_TEXVRAM.size[id] = size;
            NF_TEXVRAM.fragmented -= gfxsize; // Reduce fragmented VRAM
            organize = false; // Don't reorganize the array of free blocks
        }
        else
        {
            // All the space has been used
            NF_TEXVRAM.fragmented -= NF_TEXVRAM.size[id];
        }

        // If we have to reorganize the array of free blocks
        if (organize)
        {
            int last_reuse = NF_TEXVRAM.deleted - 1;

            // If there is more than one deleted block and it isn't the last
            // position
            if ((last_reuse > 0) && (id != last_reuse))
            {
                // Save the values of the last position here
                NF_TEXVRAM.pos[id] = NF_TEXVRAM.pos[last_reuse];
                NF_TEXVRAM.size[id] = NF_TEXVRAM.size[last_reuse];
            }

            // Reduce the number of deleted blocks
            NF_TEXVRAM.deleted--;
        }
    }
    else
    {
        // If there aren't deleted blocks, or they are too small, place the new
        // texture at the end of the used VRAM

        // Update the value of contiguous available VRAM (at the end)
        NF_TEXVRAM.inarow -= gfxsize;

        // Fail if there isn't enough contiguous VRAM
        if (NF_TEXVRAM.inarow < 0)
            NF_Error(113, "Sprites", gfxsize);

        // Transfer texture to VRAM
        NF_DmaMemCopy((void *)NF_TEXVRAM.next, NF_BUFFER_SPR256GFX[ram], gfxsize);

        // Save the address
        NF_TEX256VRAM[vram].address = NF_TEXVRAM.next;

        // Use the current address as the last address used
        NF_TEXVRAM.last = NF_TEXVRAM.next;

        // Calculate the next available address
        NF_TEXVRAM.next += gfxsize;
    }

    // Save information about this texture
    NF_TEX256VRAM[vram].size = gfxsize;
    NF_TEX256VRAM[vram].width = NF_SPR256GFX[ram].width;
    NF_TEX256VRAM[vram].height = NF_SPR256GFX[ram].height;
    NF_TEX256VRAM[vram].ramid = ram;
    NF_TEX256VRAM[vram].keepframes = keepframes;

    // Setup VRAM_B as texture memory
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);
}

void NF_Free3dSpriteGfx(u32 id)
{
    if (!NF_TEX256VRAM[id].inuse)
        NF_Error(110, "Sprite Gfx", id);

    // Let the CPU access VRAM_B
    vramSetBankB(VRAM_B_LCD);

    // Clear graphics
    memset((void *)NF_TEX256VRAM[id].address, 0, NF_TEX256VRAM[id].size);

    // Update the available VRAM
    NF_TEXVRAM.free += NF_TEX256VRAM[id].size;

    // Save the position and size of the deleted block to be reused
    NF_TEXVRAM.pos[NF_TEXVRAM.deleted] = NF_TEX256VRAM[id].address;
    NF_TEXVRAM.size[NF_TEXVRAM.deleted] = NF_TEX256VRAM[id].size;

    NF_TEXVRAM.deleted++;

    // Increment fragmented memory counter
    NF_TEXVRAM.fragmented += NF_TEX256VRAM[id].size;

    // Mark as unused
    NF_TEX256VRAM[id].inuse = false;

    // Check if VRAM is too fragmented and it has to be defragmented
    if (NF_TEXVRAM.fragmented >= (NF_TEXVRAM.inarow / 2))
        NF_Vram3dSpriteGfxDefrag();

    // Setup VRAM_B as texture memory
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);
}

void NF_Vram3dSpriteGfxDefrag(void)
{
    // Let the CPU access VRAM_B
    vramSetBankB(VRAM_B_LCD);

    // Calculate the size of the VRAM in use and create a temporary buffer
    u32 used_vram = (131072 - NF_TEXVRAM.free) + 1;
    char *buffer = calloc(used_vram, sizeof(char));
    if (buffer == NULL)
        NF_Error(102, NULL, used_vram);

    char *address[NF_3DSPRITES]; // Array of texture addresses in VRAM
    u32 size[NF_3DSPRITES];      // Array of texture sizes

    // Copy textures in use from VRAM to the temporary buffer
    u32 ram = 0; // Initial VRAM address
    for (int n = 0; n < NF_3DSPRITES; n++)
    {
        if (NF_TEX256VRAM[n].inuse)
        {
            address[n] = buffer + ram;
            size[n] = NF_TEX256VRAM[n].size;
            NF_DmaMemCopy(address[n], (void *)NF_TEX256VRAM[n].address, size[n]);
            ram += size[n]; // Next position in RAM (relative)
        }
    }

    // Initialize the VRAM information struct
    NF_TEXVRAM.free = 131072;       // Available VRAM (128 KB)
    NF_TEXVRAM.last = 0;            // Last used position
    NF_TEXVRAM.deleted = 0;         // No graphics deleted
    NF_TEXVRAM.fragmented = 0;      // Fragmented VRAM
    NF_TEXVRAM.inarow = 131072;     // Contiguous VRAM

    for (int n = 0; n < NF_3DSPRITES; n++)
    {
        NF_TEXVRAM.pos[n] = 0;
        NF_TEXVRAM.size[n] = 0;
    }

    NF_TEXVRAM.next = 0x06820000; // Next VRAM address to be used for allocations

    // Copy textures to VRAM, but packed
    for (int n = 0; n < NF_3DSPRITES; n++)
    {
        if (NF_TEX256VRAM[n].inuse)
        {
            NF_DmaMemCopy((void *)NF_TEXVRAM.next, address[n], size[n]);
            NF_TEX256VRAM[n].address = NF_TEXVRAM.next; // Next available VRAM address
            NF_TEXVRAM.free -= size[n];
            NF_TEXVRAM.inarow -= size[n];
            NF_TEXVRAM.last = NF_TEXVRAM.next; // Save this address as the last one used
            NF_TEXVRAM.next += size[n];        // Calculate the next address to use
        }
    }

    // Reassign graphics to their sprites
    for (int n = 0; n < NF_3DSPRITES; n++)
    {
        if (NF_3DSPRITE[n].inuse)
        {
            u32 gfx_address;

            NF_3DSPRITE[n].gfx = NF_TEX256VRAM[NF_3DSPRITE[n].gfxid].address;
            if (NF_TEX256VRAM[NF_3DSPRITE[n].gfxid].keepframes)
            {
                gfx_address = NF_3DSPRITE[n].gfx;
            }
            else
            {
                gfx_address = NF_3DSPRITE[n].gfx +
                              (NF_3DSPRITE[n].framesize * NF_3DSPRITE[n].frame);
            }

            // Build GPU texture command
            u32 x_size = NF_GetTextureSize(NF_3DSPRITE[n].width);
            u32 y_size = NF_GetTextureSize(NF_3DSPRITE[n].height);
            NF_3DSPRITE[n].gfx_tex_format = ((gfx_address >> 3) & 0xFFFF) |
                    (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) |
                    GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF;
        }
    }

    // Free the temporary buffer
    free(buffer);

    // Setup VRAM_B as texture memory
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);
}

void NF_Vram3dSpritePal(u32 id, u32 slot)
{
    if (id >= NF_SLOTS_SPR256PAL)
        NF_Error(106, "Sprite PAL", NF_SLOTS_SPR256PAL);

    if (NF_SPR256PAL[id].available)
        NF_Error(110, "Sprite PAL", id);

    if (slot > 31)
        NF_Error(106, "Sprite Palette Slot", 31);

    // Copy palette to VRAM to the right slot
    u32 address = 0x06890000 + (slot * 256 * 2);
    vramSetBankF(VRAM_F_LCD); // Let the CPU access VRAM_F
    NF_DmaMemCopy((void *)address, NF_BUFFER_SPR256PAL[id], NF_SPR256PAL[id].size);
    vramSetBankF(VRAM_F_TEX_PALETTE); // Setup VRAM_F for texture palettes

    // Save parameters and mark as used
    NF_TEXPALSLOT[slot].inuse = true;
    NF_TEXPALSLOT[slot].ramslot = id;
}

void NF_Create3dSprite(u32 id, u32 gfx, u32 pal, s32 x, s32 y)
{
    if (id > (NF_3DSPRITES - 1))
        NF_Error(106, "3D sprite", NF_3DSPRITES - 1);

    if (gfx > (NF_3DSPRITES - 1))
        NF_Error(106, "3D sprite gfx", NF_3DSPRITES - 1);

    if (!NF_TEX256VRAM[gfx].inuse)
        NF_Error(111, "3D Sprite GFX", gfx);

    if (pal > 31)
        NF_Error(106, "3D Sprite Palette Slot", 31);

    if (!NF_TEXPALSLOT[pal].inuse)
        NF_Error(111, "3D Sprite PAL", pal);

    if (NF_3DSPRITE[id].inuse)
        NF_Delete3dSprite(id);

    // Calculate palette address and save it in the sprite struct
    u32 pal_address = pal * 256 * 2;
    NF_3DSPRITE[id].pal = pal_address; // Offset to the address from the base of VRAM_F
    NF_3DSPRITE[id].palid = pal;       // Number of palette
    // GPU palette comand format
    NF_3DSPRITE[id].gfx_pal_format = (pal_address >> 4) & 0x1FFF;

    // Calculate texture address and save it in the sprite struct
    u32 gfx_address = NF_TEX256VRAM[gfx].address;
    NF_3DSPRITE[id].gfx = gfx_address; // Offset to the address from the base of VRAM_B
    NF_3DSPRITE[id].gfxid = gfx;       // Number of the graphics used
    u16 x_size = NF_GetTextureSize(NF_TEX256VRAM[gfx].width);
    u16 y_size = NF_GetTextureSize(NF_TEX256VRAM[gfx].height);
    // GPU texture command format
    NF_3DSPRITE[id].gfx_tex_format = ((gfx_address >> 3) & 0xFFFF) |
            (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) |
            GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF;

    // Save sprite parameters
    NF_3DSPRITE[id].x = x;
    NF_3DSPRITE[id].y = y;
    NF_3DSPRITE[id].z = 0;
    NF_3DSPRITE[id].width = NF_TEX256VRAM[gfx].width;
    NF_3DSPRITE[id].height = NF_TEX256VRAM[gfx].height;
    NF_3DSPRITE[id].framesize = NF_TEX256VRAM[gfx].framesize;
    NF_3DSPRITE[id].lastframe = NF_TEX256VRAM[gfx].lastframe;
    NF_3DSPRITE[id].prio = NF_CREATED_3DSPRITE.total;
    NF_3DSPRITE[id].poly_id = 0;
    NF_3DSPRITE[id].alpha = 31;
    NF_3DSPRITE[id].rx = 0;
    NF_3DSPRITE[id].ry = 0;
    NF_3DSPRITE[id].rz = 0;
    NF_3DSPRITE[id].rot = false;
    NF_3DSPRITE[id].sx = 64 << 6;
    NF_3DSPRITE[id].sy = 64 << 6;
    NF_3DSPRITE[id].scale = false;
    NF_3DSPRITE[id].frame = 0;
    NF_3DSPRITE[id].newframe = 0;

    // Register it as created
    NF_CREATED_3DSPRITE.id[NF_CREATED_3DSPRITE.total] = id;
    NF_CREATED_3DSPRITE.total++;
    NF_3DSPRITE[id].show = true;
    NF_3DSPRITE[id].inuse = true;
}

void NF_Delete3dSprite(u32 id)
{
    if (id > (NF_3DSPRITES - 1))
        NF_Error(106, "3D sprite", NF_3DSPRITES - 1);

    if (!NF_3DSPRITE[id].inuse)
        NF_Error(112, "3D sprite", id);

    // Mark sprite as unused
    NF_3DSPRITE[id].inuse = false;

    // Remove the selected sprite from the queue

    if (NF_CREATED_3DSPRITE.total > 1)
    {
        // If there are more than one sprite
        int n2 = 0;
        for (int n1 = 0; n1 < NF_CREATED_3DSPRITE.total; n1++)
        {
            // If this is any other sprite than the selected one, copy it to the
            // temporary array
            if (NF_CREATED_3DSPRITE.id[n1] != id)
            {
                NF_CREATED_3DSPRITE.bck[n2] = NF_CREATED_3DSPRITE.id[n1];
                n2++;
            }
        }

        NF_CREATED_3DSPRITE.total--;

        // Copy the temporary array to the active array
        for (int n1 = 0; n1 < NF_CREATED_3DSPRITE.total; n1++)
            NF_CREATED_3DSPRITE.id[n1] = NF_CREATED_3DSPRITE.bck[n1];
    }
    else
    {
        // If this is the last sprite
        NF_CREATED_3DSPRITE.id[0] = 0;
        NF_CREATED_3DSPRITE.total = 0;
    }
}

void NF_Sort3dSprites(void)
{
    // Return if the number of sprites isn't big enough
    if (NF_CREATED_3DSPRITE.total < 2)
        return;

    // Sort queue
    for (int n1 = 0; n1 < (NF_CREATED_3DSPRITE.total - 1); n1++)
    {
        for (int n2 = n1 + 1; n2 < NF_CREATED_3DSPRITE.total; n2++)
        {
            if (NF_CREATED_3DSPRITE.id[n2] < NF_CREATED_3DSPRITE.id[n1])
            {
                // Swap IDs
                u32 id2 = NF_CREATED_3DSPRITE.id[n2];
                u32 id1 = NF_CREATED_3DSPRITE.id[n1];

                NF_CREATED_3DSPRITE.id[n1] = id2;
                NF_CREATED_3DSPRITE.id[n2] = id1;

                // Update priorities
                NF_3DSPRITE[id2].prio = n1;
                NF_3DSPRITE[id1].prio = n2;
            }
        }
    }
}

void NF_Set3dSpritePriority(u32 id, u32 prio)
{
    // Create a backup of the queue
    for (int n = 0; n < NF_CREATED_3DSPRITE.total; n++)
        NF_CREATED_3DSPRITE.bck[n] = NF_CREATED_3DSPRITE.id[n];

    NF_CREATED_3DSPRITE.id[prio] = id;
    NF_3DSPRITE[id].prio = prio;

    // Add sprite to the right position in the queue based on the priority
    u32 a = 0;
    u32 b = 0;

    for (int n = 0; n < NF_CREATED_3DSPRITE.total; n++)
    {
        if ((a != prio) && (NF_CREATED_3DSPRITE.bck[b] != id))
        {
            NF_CREATED_3DSPRITE.id[a] = NF_CREATED_3DSPRITE.bck[b];
            NF_3DSPRITE[NF_CREATED_3DSPRITE.id[a]].prio = a;
            a++;
            b++;
        }
        else
        {
            // Skip entries with the same priority
            if (a == prio)
                a++;

            // If the ID matches, skip it
            if (NF_CREATED_3DSPRITE.bck[b] == id)
                b++;
        }
    }
}

void NF_Swap3dSpritePriority(u32 id_a, u32 id_b)
{
    // Swap priorities
    u32 prio_a = NF_3DSPRITE[id_b].prio;
    u32 prio_b = NF_3DSPRITE[id_a].prio;

    NF_3DSPRITE[id_a].prio = prio_a;
    NF_3DSPRITE[id_b].prio = prio_b;

    // Modify the sprite queue
    NF_CREATED_3DSPRITE.id[prio_a] = id_a;
    NF_CREATED_3DSPRITE.id[prio_b] = id_b;
}

void NF_Set3dSpriteFrame(u32 id, u32 frame)
{
    if (id > (NF_3DSPRITES - 1))
        NF_Error(106, "3D sprite", NF_3DSPRITES - 1);

    if (!NF_3DSPRITE[id].inuse)
        NF_Error(112, "3D sprite", id);

    if (frame > NF_3DSPRITE[id].lastframe)
        NF_Error(106, "Sprite frame", NF_3DSPRITE[id].lastframe);

    if (NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].keepframes)
    {
        // If the sprite frames are kept in RAM, mark the new frame as the
        // desired frame so that it is copied to VRAM during the update of all
        // 3D sprites.
        NF_3DSPRITE[id].newframe = frame;
    }
    else
    {
        // If all frames are already in VRAM update the information in the
        // sprite structure.

        u32 gfx_address = NF_3DSPRITE[id].gfx + (NF_3DSPRITE[id].framesize * frame);
        u16 x_size = NF_GetTextureSize(NF_3DSPRITE[id].width);
        u16 y_size = NF_GetTextureSize(NF_3DSPRITE[id].height);
        NF_3DSPRITE[id].gfx_tex_format = ((gfx_address >> 3) & 0xFFFF) |
                    (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) |
                    GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF;
        NF_3DSPRITE[id].frame = frame;
        NF_3DSPRITE[id].newframe = frame;
    }
}

void NF_Draw3dSprites(void)
{
    // If there are no 3D sprites, return
    if (NF_CREATED_3DSPRITE.total == 0)
        return;

    for (int n = 0; n < NF_CREATED_3DSPRITE.total; n++)
    {
        u32 id = NF_CREATED_3DSPRITE.id[n];

        if (!NF_3DSPRITE[id].inuse)
            continue;

        if (!NF_3DSPRITE[id].show)
            continue;

        // Calculate the current Z
        u32 z = n + NF_3DSPRITE[id].z;

        // Apply alpha value and disable culling so that the sprite is drawn
        // regardless of the orientation
        glPolyFmt(POLY_ALPHA(NF_3DSPRITE[id].alpha) | POLY_ID(NF_3DSPRITE[id].poly_id) |
                             POLY_CULL_NONE);

        if (NF_3DSPRITE[id].rot || NF_3DSPRITE[id].scale)
        {
            glPushMatrix();

            // Translate to the center of the sprite
            u32 x = NF_3DSPRITE[id].x + (NF_3DSPRITE[id].width / 2);
            u32 y = NF_3DSPRITE[id].y + (NF_3DSPRITE[id].height / 2);

            glTranslatef32(x, y, z);

            if (NF_3DSPRITE[id].rot)
            {
                glRotateXi(NF_3DSPRITE[id].rx);
                glRotateYi(NF_3DSPRITE[id].ry);
                glRotateZi(NF_3DSPRITE[id].rz);
            }

            if (NF_3DSPRITE[id].scale)
                glScalef32(NF_3DSPRITE[id].sx, NF_3DSPRITE[id].sy, 0);

            // Do the opposite translation to complete the rotation and scale
            glTranslatef32(-x, -y, -z);
        }

        // Apply texture and palette
        GFX_PAL_FORMAT = NF_3DSPRITE[id].gfx_pal_format;
        GFX_TEX_FORMAT = NF_3DSPRITE[id].gfx_tex_format;

        // Calculate vertices
        u32 x1 = NF_3DSPRITE[id].x;
        u32 x2 = NF_3DSPRITE[id].x + NF_3DSPRITE[id].width;
        u32 y1 = NF_3DSPRITE[id].y;
        u32 y2 = NF_3DSPRITE[id].y + NF_3DSPRITE[id].height;

        // Draw polygon
        glBegin(GL_QUAD);

            // Top left
            glTexCoord2t16(inttot16(0), inttot16(0));
            glVertex3v16(x1, y1, z);

            // Bottom left
            glTexCoord2t16(inttot16(0), inttot16(NF_3DSPRITE[id].height));
            glVertex3v16(x1, y2, z);

            // Bottom right
            glTexCoord2t16(inttot16(NF_3DSPRITE[id].width),
                           inttot16(NF_3DSPRITE[id].height));
            glVertex3v16(x2, y2, z);

            // Top right
            glTexCoord2t16(inttot16(NF_3DSPRITE[id].width), inttot16(0));
            glVertex3v16(x2, y1, z);

        glEnd();

        // Restore matrix if we have done a transformation before
        if (NF_3DSPRITE[id].rot || NF_3DSPRITE[id].scale)
            glPopMatrix(1);
    }
}

void NF_Update3dSpritesGfx(void)
{
    // If there is nothing to update, return
    if (NF_CREATED_3DSPRITE.total == 0)
        return;

    // This function copies textures from RAM to VRAM if the animation frame has
    // changed and all frames haven't been copied to VRAM.

    // Let the CPU access VRAM_B
    vramSetBankB(VRAM_B_LCD);

    for (int n = 0; n < NF_CREATED_3DSPRITE.total; n++)
    {
        u32 id = NF_CREATED_3DSPRITE.id[n];

        // If frames are only stored in RAM and the frame has to change
        if (NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].keepframes &&
            (NF_3DSPRITE[id].frame != NF_3DSPRITE[id].newframe))
        {
            u32 ramid = NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].ramid;
            char *source = NF_BUFFER_SPR256GFX[ramid] +
                           (NF_3DSPRITE[id].framesize * NF_3DSPRITE[id].newframe);
            u32 destination = NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].address;
            NF_DmaMemCopy((void *)destination, source, NF_3DSPRITE[id].framesize);

            // Update the current frame
            NF_3DSPRITE[id].frame = NF_3DSPRITE[id].newframe;
        }
    }

    // Set VRAM_B as a texture bank
    vramSetBankB(VRAM_B_TEXTURE_SLOT1);
}

void NF_Rotate3dSprite(u32 id, s32 x, s32 y, s32 z)
{
    NF_3DSPRITE[id].rx = x << 6;
    NF_3DSPRITE[id].ry = y << 6;
    NF_3DSPRITE[id].rz = z << 6;

    // Flag the sprite as rotated if the scale values aren't the default
    if (NF_3DSPRITE[id].rx | NF_3DSPRITE[id].ry | NF_3DSPRITE[id].rz)
        NF_3DSPRITE[id].rot = true;
    else
        NF_3DSPRITE[id].rot = false;
}

void NF_Scale3dSprite(u32 id, u32 x, u32 y)
{
    NF_3DSPRITE[id].sx = x << 6;
    NF_3DSPRITE[id].sy = y << 6;

    // Flag the sprite as scaled if the scale values aren't the default
    if ((NF_3DSPRITE[id].sx != (64 << 6)) || (NF_3DSPRITE[id].sy != (64 << 6)))
        NF_3DSPRITE[id].scale = true;
    else
        NF_3DSPRITE[id].scale = false;
}

void NF_Blend3dSprite(u32 sprite, u32 poly_id, u32 alpha)
{
    // Disable alpha if the alpha value is 31 or the polygon ID is 0
    if ((poly_id == 0) || (alpha == 31))
    {
        NF_3DSPRITE[sprite].poly_id = 0;
        NF_3DSPRITE[sprite].alpha = 31;
    }
    else
    {
        NF_3DSPRITE[sprite].poly_id = poly_id;
        NF_3DSPRITE[sprite].alpha = alpha;
    }
}

void NF_3dSpritesLayer(u32 layer)
{
    // Clear priority bits from the control registers
    REG_BG0CNT &= 0xFFFC;
    REG_BG1CNT &= 0xFFFC;
    REG_BG2CNT &= 0xFFFC;
    REG_BG3CNT &= 0xFFFC;

    // Reorder layers based on the requested priority
    switch (layer)
    {
        default:
        case 0:
            REG_BG0CNT |= BG_PRIORITY_0;
            REG_BG1CNT |= BG_PRIORITY_1;
            REG_BG2CNT |= BG_PRIORITY_2;
            REG_BG3CNT |= BG_PRIORITY_3;
            break;
        case 1:
            REG_BG0CNT |= BG_PRIORITY_1;
            REG_BG1CNT |= BG_PRIORITY_0;
            REG_BG2CNT |= BG_PRIORITY_2;
            REG_BG3CNT |= BG_PRIORITY_3;
            break;
        case 2:
            REG_BG0CNT |= BG_PRIORITY_2;
            REG_BG1CNT |= BG_PRIORITY_0;
            REG_BG2CNT |= BG_PRIORITY_1;
            REG_BG3CNT |= BG_PRIORITY_3;
            break;
        case 3:
            REG_BG0CNT |= BG_PRIORITY_3;
            REG_BG1CNT |= BG_PRIORITY_0;
            REG_BG2CNT |= BG_PRIORITY_1;
            REG_BG3CNT |= BG_PRIORITY_2;
            break;
    }
}

void NF_3dSpriteEditPalColor(u32 pal, u32 number, u32 r, u32 g, u32 b)
{
    if (!NF_TEXPALSLOT[pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    // Pack RGB value
    u32 rgb = r | (g << 5) | (b << 10);

    u32 hibyte = rgb >> 8;
    u32 lobyte = rgb - (hibyte << 8);

    *(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + (number * 2)) = lobyte;
    *(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + ((number * 2) + 1)) = hibyte;
}

void NF_3dSpriteUpdatePalette(u32 pal)
{
    if (!NF_TEXPALSLOT[pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    // Get slot where the palette is stored in RAM
    u32 slot = NF_TEXPALSLOT[pal].ramslot;

    // Update palette in VRAM
    u32 address = 0x06890000 + (pal * 256 * 2);
    vramSetBankF(VRAM_F_LCD); // Let the CPU access VRAM_F
    NF_DmaMemCopy((void *)address, NF_BUFFER_SPR256PAL[slot], NF_SPR256PAL[slot].size);
    vramSetBankF(VRAM_F_TEX_PALETTE); // Setup VRAM_F for texture palettes
}

void NF_3dSpriteGetPalColor(u32 pal, u32 number, u8 *r, u8 *g, u8 *b)
{
    if (!NF_TEXPALSLOT[pal].inuse)
        NF_Error(111, "Sprite palette", pal);

    u32 lobyte = *(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + (number * 2));
    u32 hibyte = *(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + ((number * 2) + 1));

    // Build RGB value
    u32 rgb = (hibyte << 8) + lobyte;

    *r = rgb & 0x1F;
    *g = (rgb >> 5) & 0x1F;
    *b = (rgb >> 10) & 0x1F;
}

void NF_3dSpriteSetDepth(u32 id, s32 z)
{
    // Limit range
    if (z < -512)
        z = -512;
    if (z > 512)
        z = 512;

    NF_3DSPRITE[id].z = z;
}
