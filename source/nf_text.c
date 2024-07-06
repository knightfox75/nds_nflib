// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 8x8 font text support functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_text.h"
#include "nf_tiledbg.h"

// Structs that hold information about all text layers
NF_TYPE_TEXT_INFO NF_TEXT[2][4];

void NF_InitTextSys(int screen)
{
    for (int n = 0; n < 4; n++)
    {
        NF_TEXT[screen][n].width = 0;
        NF_TEXT[screen][n].height = 0;
        NF_TEXT[screen][n].rotation = 0;
        NF_TEXT[screen][n].slot = 255;
        NF_TEXT[screen][n].pal = 0;
        NF_TEXT[screen][n].exist = false;
        NF_TEXT[screen][n].update = false;
    }
}

void NF_LoadTextFont(const char *file, const char *name, u32 width, u32 height,
                     u32 rotation)
{
    // Look for a free slot
    u32 slot = 255;
    for (int n = 0; n < NF_SLOTS_TBG; n ++)
    {
        // If a slot is free, mark it as in use and stop the search
        if (NF_TILEDBG[n].available)
        {
            NF_TILEDBG[n].available = false;
            slot = n;
            break;
        }
    }

    // If there are no free slots, fail
    if (slot == 255)
        NF_Error(103, "Tiled Bg", NF_SLOTS_TBG);

    // Verify that the background is a multiple of 256 pixels (32 tiles)
    if (((width % 256) != 0) || ((height % 256) != 0))
        NF_Error(115, file, 0);

    // Free buffers if they are in use
    free(NF_BUFFER_BGMAP[slot]);
    NF_BUFFER_BGMAP[slot] = NULL;
    free(NF_BUFFER_BGTILES[slot]);
    NF_BUFFER_BGTILES[slot] = NULL;
    free(NF_BUFFER_BGPAL[slot]);
    NF_BUFFER_BGPAL[slot] = NULL;

    // File path
    char filename[256];

    // Load .FNT/.IMG file
    snprintf(filename, sizeof(filename), "%s/%s.fnt", NF_ROOTFOLDER, file);
    FILE *file_id = fopen(filename, "rb");
    if (file_id == NULL) // If the file can't be opened
    {
        snprintf(filename, sizeof(filename), "%s/%s.img", NF_ROOTFOLDER, file);
        file_id = fopen(filename, "rb");
        if (file_id == NULL) // If the file can't be opened
            NF_Error(101, filename, 0);
    }

    // Get file size
    NF_TILEDBG[slot].tilesize = NF_TEXT_FONT_CHARS * 64;

    // Allocate space in RAM
    NF_BUFFER_BGTILES[slot] = malloc(NF_TILEDBG[slot].tilesize);
    if (NF_BUFFER_BGTILES[slot] == NULL) // If there is not enough RAM
        NF_Error(102, NULL, NF_TILEDBG[slot].tilesize);

    // Read file to RAM
    fread(NF_BUFFER_BGTILES[slot], 1, NF_TILEDBG[slot].tilesize, file_id);
    fclose(file_id);

    // Rotate graphics if requested
    if (rotation > 0)
    {
        for (int n = 0; n < NF_TEXT_FONT_CHARS; n++)
            NF_RotateTileGfx(slot, n, rotation);
    }

    // Create an empty map in RAM. Each map entry is 2 bytes in size
    NF_TILEDBG[slot].mapsize = ((width / 8) * (height / 8)) * 2;

    // Allocate space in RAM and zero it (calloc returns zeroed memory)
    NF_BUFFER_BGMAP[slot] = calloc(NF_TILEDBG[slot].mapsize, sizeof(char));
    if (NF_BUFFER_BGMAP[slot] == NULL) // If there is not enough RAM
        NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);

    // Load .PAL file
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_BGPAL[slot], &NF_TILEDBG[slot].palsize, 0);

    // Save background name
    snprintf(NF_TILEDBG[slot].name, sizeof(NF_TILEDBG[slot].name), "%s", name);

    // Save the size
    NF_TILEDBG[slot].width = width;
    NF_TILEDBG[slot].height = height;
}

void NF_UnloadTextFont(const char *name)
{
    NF_UnloadTiledBg(name);
}

void NF_CreateTextLayer(int screen, u32 layer, u32 rotation, const char *name)
{
    // Create a background to use it as a text layer
    NF_CreateTiledBg(screen, layer, name);

    u32 slot = 255;

    // Look for the tiled background with the specified name
    for (int n = 0; n < NF_SLOTS_TBG; n ++)
    {
        if (strcmp(name, NF_TILEDBG[n].name) == 0)
        {
            slot = n;
            break;
        }
    }

    // If it hasn't been found, fail
    if (slot == 255)
        NF_Error(103, "Tiled BG", NF_SLOTS_TBG);

    NF_TEXT[screen][layer].rotation = rotation;

    // Save the background size in tiles (save the index of the last row/column)
    NF_TEXT[screen][layer].width = (NF_TILEDBG[slot].width / 8) - 1;
    NF_TEXT[screen][layer].height = (NF_TILEDBG[slot].height / 8) - 1;

    // Save slot where the font is stored
    NF_TEXT[screen][layer].slot = slot;

    // Mark layer as used
    NF_TEXT[screen][layer].exist = true;
}

void NF_DeleteTextLayer(int screen, u32 layer)
{
    // Verify that the selected text layer exists
    if (!NF_TEXT[screen][layer].exist)
        NF_Error(114, NULL, screen);

    // Delete the tiled background used as text layer
    NF_DeleteTiledBg(screen, layer);

    NF_TEXT[screen][layer].rotation = 0;
    NF_TEXT[screen][layer].width = 0;
    NF_TEXT[screen][layer].height = 0;

    // Mark layer as unused
    NF_TEXT[screen][layer].exist = false;
}

void NF_WriteText(int screen, u32 layer, u32 x, u32 y, const char *text)
{
    // Verify that the selected text layer exists
    if (!NF_TEXT[screen][layer].exist)
        NF_Error(114, NULL, screen);

    u32 tsize = strlen(text); // Size of the temporary string buffer
    u8 *string = malloc(tsize); // Temporary string buffer
    if (string == NULL)
        NF_Error(102, NULL, tsize);

    // Store the text string in the temporary buffer
    for (u32 n = 0; n < tsize; n++)
    {
        int value = 0;

        // If newline found, put the character to the newline. If not, skip the
        // first 32 characters.
        if (text[n] == '\n')
            value = '\n';
        else
            value = text[n] - 32;

        if (value < 0)
            value = 0;

        string[n] = value;

        // Handle special characters (newline characters are special too)
        if ((string[n] > 95) || (string[n] == 10))
        {
            switch (text[n])
            {
                case '\n':
                    string[n] = 200;
                    break;

                case 199: // Ç
                    string[n] = 96;
                    break;
                case 231: // ç
                    string[n] = 97;
                    break;
                case 209: // Ñ
                    string[n] = 98;
                    break;
                case 241: // ñ
                    string[n] = 99;
                    break;

                case 193: // Á
                    string[n] = 100;
                    break;
                case 201: // É
                    string[n] = 101;
                    break;
                case 205: // Í
                    string[n] = 102;
                    break;
                case 211: // Ó
                    string[n] = 103;
                    break;
                case 218: // Ú
                    string[n] = 104;
                    break;

                case 225: // á
                    string[n] = 105;
                    break;
                case 233: // é
                    string[n] = 106;
                    break;
                case 237: // í
                    string[n] = 107;
                    break;
                case 243: // ó
                    string[n] = 108;
                    break;
                case 250: // ú
                    string[n] = 109;
                    break;

                case 239: // ï
                    string[n] = 110;
                    break;
                case 252: // ü
                    string[n] = 111;
                    break;

                case 161: // ¡
                    string[n] = 112;
                    break;
                case 191: // ¿
                    string[n] = 113;
                    break;

                default:
                    string[n] = 0;
                    break;
            }
        }
    }

    // Variables to calculate the position of the text
    s32 tx, ty;

    // Write text in the background map, according to the specified rotation
    switch (NF_TEXT[screen][layer].rotation)
    {
        case 0: // No rotation

            tx = x;
            ty = y;

            for (u32 n = 0; n < tsize; n++)
            {
                // If it's a valid character, put character
                if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR)
                {
                    NF_SetTileOfMap(screen,layer, tx, ty,
                                    (NF_TEXT[screen][layer].pal << 12) | string[n]);
                    tx++;
                }

                // If the end of the line is reached or a newline character is found
                if ((tx > NF_TEXT[screen][layer].width) || (string[n] == 200))
                {
                    tx = 0;
                    ty++;
                    // If the last row is reached, return to the first one
                    if (ty > NF_TEXT[screen][layer].height)
                        ty = 0;
                }
            }
            break;

        case 1: // 90 degrees of clockwise rotation

            tx = NF_TEXT[screen][layer].width - y;
            ty = x;

            for (u32 n = 0; n < tsize; n++)
            {
                // If it's a valid character, put character
                if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR)
                {
                    NF_SetTileOfMap(screen,layer, tx, ty,
                                    (NF_TEXT[screen][layer].pal << 12) | string[n]);
                    ty++;
                }

                // If the end of the line is reached or a newline character is found
                if ((ty > NF_TEXT[screen][layer].height) || (string[n] == 200))
                {
                    ty = 0;
                    tx--;

                    // If the last row is reached, return to the first one
                    if (tx < 0)
                        tx = NF_TEXT[screen][layer].width;
                }
            }
            break;

        case 2: // 90 degrees of counter-clockwise rotation

            tx = y;
            ty = NF_TEXT[screen][layer].height - x;

            for (u32 n = 0; n < tsize; n ++)
            {
                // If it's a valid character, put character
                if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR)
                {
                    NF_SetTileOfMap(screen,layer, tx, ty,
                                    (NF_TEXT[screen][layer].pal << 12) | string[n]);
                    ty --;
                }

                // If the end of the line is reached or a newline character is found
                if ((ty < 0) || (string[n] == 200))
                {
                    ty = NF_TEXT[screen][layer].height;
                    tx++;

                    // If the last row is reached, return to the first one
                    if (tx > NF_TEXT[screen][layer].width)
                        tx = 0;
                }
            }
            break;
    }

    // Mark this layer as needing an update
    NF_TEXT[screen][layer].update = true;

    // Free temporary buffer
    free(string);
}

void NF_UpdateTextLayers(void)
{
    for (int screen = 0; screen < 2; screen++)
    {
        for (int layer = 0; layer < 4; layer++)
        {
            // If the layer needs to be updated, update it and mark it as not
            // requiring an update.
            if (NF_TEXT[screen][layer].update)
            {
                NF_UpdateVramMap(screen, layer);
                NF_TEXT[screen][layer].update = false;
            }
        }
    }
}

void NF_ClearTextLayer(int screen, u32 layer)
{
    // Verify that the selected text layer exists
    if (!NF_TEXT[screen][layer].exist)
        NF_Error(114, NULL, screen);

    // Calculate buffer size (2 bytes per tile)
    u32 size = (NF_TEXT[screen][layer].width + 1) * (NF_TEXT[screen][layer].height + 1) * 2;

    // Zero the map
    memset(NF_BUFFER_BGMAP[NF_TEXT[screen][layer].slot], 0, size);

    // Mark this layer as needing an update
    NF_TEXT[screen][layer].update = true;
}

void NF_DefineTextColor(int screen, u32 layer, u32 color, u32 r, u32 g, u32 b)
{
    // Verify that the selected text layer exists
    if (!NF_TEXT[screen][layer].exist)
        NF_Error(114, NULL, screen);

    // Pack RGB value
    u32 rgb = r | (g << 5) | (b << 10);

    // Modify the palette of the selected screen
    if (screen == 0)
    {
        vramSetBankE(VRAM_E_LCD); // Enable CPU access to VRAM_E
        u32 address = 0x06880000 + (layer << 13) + (color * 256 * 2); // First color
        *((u16*)address) = (u16)0xFF00FF;
        address = 0x06880000 + (layer << 13) + (color * 256 * 2) + 2; // Second color
        *((u16*)address) = rgb;
        vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    }
    else
    {
        vramSetBankH(VRAM_H_LCD); // Enable CPU access to VRAM_H
        u32 address = 0x06898000 + (layer << 13) + (color * 256 * 2); // First color
        *((u16*)address) = (u16)0xFF00FF;
        address = 0x06898000 + (layer << 13) + (color * 256 * 2) + 2; // Second color
        *((u16*)address) = rgb;
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    }
}
