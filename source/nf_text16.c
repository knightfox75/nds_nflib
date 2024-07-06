// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 8x16 font text support functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_text16.h"
#include "nf_text.h"
#include "nf_tiledbg.h"

void NF_LoadTextFont16(const char *file, const char *name, u32 width, u32 height,
                       u32 rotation)
{
    // Look for a free slot and mark it as in use
    int slot = 255;
    for (int n = 0; n < NF_SLOTS_TBG; n++)
    {
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

    // Verify that the size of the background is a multiple of 256 px (32 tiles)
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

    // Get file size. 256 colors, so 1 byte per pixel (8x16 pixels)
    NF_TILEDBG[slot].tilesize = NF_TEXT_FONT_CHARS_16 * 8 * 16;

    // Allocate space in RAM
    NF_BUFFER_BGTILES[slot] = malloc(NF_TILEDBG[slot].tilesize);
    if (NF_BUFFER_BGTILES[slot] == NULL)
        NF_Error(102, NULL, NF_TILEDBG[slot].tilesize);

    // Load file to RAM
    fread(NF_BUFFER_BGTILES[slot], 1, NF_TILEDBG[slot].tilesize, file_id);
    fclose(file_id);

    // Rotate graphics if required
    if (rotation > 0)
    {
        for (int n = 0; n < (NF_TEXT_FONT_CHARS_16 * 2); n++)
            NF_RotateTileGfx(slot, n, rotation);
    }

    // Create an empty map in RAM. Each element in the map is 2 bytes.
    NF_TILEDBG[slot].mapsize = ((width / 8) * (height / 8)) * 2;

    // Allocate space in RAM and zero it in case the file needs padding
    NF_BUFFER_BGMAP[slot] = calloc(NF_TILEDBG[slot].mapsize, sizeof(char));
    if (NF_BUFFER_BGMAP[slot] == NULL)
        NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);

    // Load .PAL file
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_BGPAL[slot], &NF_TILEDBG[slot].palsize, 256 * 2);

    // Save the name of the background
    snprintf(NF_TILEDBG[slot].name, sizeof(NF_TILEDBG[slot].name), "%s", name);

    // And its size
    NF_TILEDBG[slot].width = width;
    NF_TILEDBG[slot].height = height;
}

void NF_CreateTextLayer16(int screen, u32 layer, u32 rotation, const char* name)
{
    // Create a background to use it as a text layer
    NF_CreateTiledBg(screen, layer, name);

    u32 slot = 255;

    // Look for the tiled background with the specified name
    for (int n = 0; n < NF_SLOTS_TBG; n++)
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
    switch (rotation)
    {
        case 0: // No rotation
            NF_TEXT[screen][layer].width = (NF_TILEDBG[slot].width / 8) - 1;
            NF_TEXT[screen][layer].height = (NF_TILEDBG[slot].height / (8 * 2)) - 1;
            break;
        case 1: // 90 degrees clockwise
            NF_TEXT[screen][layer].width = (NF_TILEDBG[slot].width / (8 * 2)) - 1;
            NF_TEXT[screen][layer].height = (NF_TILEDBG[slot].height / 8) - 1;
            break;
        case 2: // 90 degrees counter-clockwise
            NF_TEXT[screen][layer].width = (NF_TILEDBG[slot].width / (8 * 2)) - 1;
            NF_TEXT[screen][layer].height = (NF_TILEDBG[slot].height / 8) - 1;
            break;
    }

    // Save slot where the font is stored
    NF_TEXT[screen][layer].slot = slot;

    // Mark layer as used
    NF_TEXT[screen][layer].exist = true;
}

void NF_WriteText16(int screen, u32 layer, u32 x, u32 y, const char *text)
{
    // Verify that the text layer exists
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
        case 0: // No rotacion

            tx = x;
            ty = y;

            for (u32 n = 0; n < tsize; n++)
            {
                // If it's a valid character, put character
                if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR_16)
                {
                    // Calculate the position in the tilemap
                    s32 pos_x = tx;
                    s32 pos_y = ty * 2;

                    // Print character
                    u32 value = (string[n] & ~31) + string[n];
                    NF_SetTileOfMap(screen,layer, pos_x, pos_y,
                                    (NF_TEXT[screen][layer].pal << 12) | value);
                    NF_SetTileOfMap(screen,layer, pos_x, pos_y + 1,
                                    (NF_TEXT[screen][layer].pal << 12) | (value + 32));
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
                if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR_16)
                {
                    // Calculate the position in the tilemap
                    s32 pos_x = tx * 2;
                    s32 pos_y = ty;

                    // Print character
                    u32 value = (string[n] & ~31) + string[n];
                    NF_SetTileOfMap(screen,layer, pos_x, pos_y,
                                    (NF_TEXT[screen][layer].pal << 12) | value);
                    NF_SetTileOfMap(screen,layer, pos_x - 1, pos_y,
                                    (NF_TEXT[screen][layer].pal << 12) | (value + 32));
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

            for (u32 n = 0; n < tsize; n++)
            {
                // If it's a valid character, put character
                if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR_16)
                {
                    s32 pos_x = tx * 2;
                    s32 pos_y = ty;

                    // Print the character
                    u32 value = (string[n] & ~31) + string[n];
                    NF_SetTileOfMap(screen,layer, pos_x, pos_y,
                                    (NF_TEXT[screen][layer].pal << 12) | value);
                    NF_SetTileOfMap(screen,layer, pos_x + 1, pos_y,
                                    (NF_TEXT[screen][layer].pal << 12) | (value + 32));
                    ty--;
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

        default:
            break;
    }

    // Mark this layers as requiring an update
    NF_TEXT[screen][layer].update = true;

    // Free temporary  buffer
    free(string);
}

void NF_ClearTextLayer16(int screen, u32 layer)
{
    // Verify that the text layer exists
    if (!NF_TEXT[screen][layer].exist)
        NF_Error(114, NULL, screen);

    // Calculate size of the buffer
    u32 w = NF_TEXT[screen][layer].width + 1;
    u32 h = NF_TEXT[screen][layer].height + 1;
    u32 size = w * h * 4; // 2 tiles per character, 2 bytes per tile

    // Clear the text layer
    memset(NF_BUFFER_BGMAP[NF_TEXT[screen][layer].slot], 0, size);

    // Mark this layers as requiring an update
    NF_TEXT[screen][layer].update = true;
}
