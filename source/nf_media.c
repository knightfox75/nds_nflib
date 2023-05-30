// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Media file loading functions
// http://www.nightfoxandco.com/

#include <stdio.h>

#include <nds.h>

#include "nf_basic.h"
#include "nf_bitmapbg.h"
#include "nf_media.h"

void NF_LoadBMP(const char *file, u32 slot)
{
    // Temporary buffers
    char *buffer = NULL;

    // BMP header
    typedef struct {
        u32 bmp_size;       // Size of the file in bytes
        u16 res_a;          // Reserved
        u16 res_b;          // Reserved
        u32 offset;         // Offset where data starts
        u32 header_size;    // Size of the header in bytes (40 bytes)
        u32 width;          // Image width in pixels
        u32 height;         // Image height in pixels
        u16 color_planes;   // Number of color planes
        u16 bpp;            // Bits per pixel
        u32 compression;    // Compression
        u32 raw_size;       // Size of raw data after the header
        u32 dpi_hor;        // Dots per inch (horizontal)
        u32 dpi_ver;        // Dots per inch (vertical)
        u32 pal_colors;     // Number of colors in the palette
        u32 imp_colors;     // Important colors
    } bmp_header_info;

    bmp_header_info bmp_header = { 0 };

    // File path
    char filename[256];

    // Load BMP file
    snprintf(filename, sizeof(filename), "%s/%s.bmp", NF_ROOTFOLDER, file);
    FILE *file_id = fopen(filename, "rb");
    if (file_id == NULL) // If the file can't be opened
        NF_Error(101, filename, 0);

    // Read magic string "BM" (0x00 - 0x01)
    char magic_id[2];
    fread(magic_id, 1, 2, file_id);
    if ((magic_id[0] != 'B') || (magic_id[1] != 'M')) // If not a valid BMP file
        NF_Error(101, "BMP", 0);

    // Read the rest of the BMP header (0x02 - 0x36)
    fread((void *)&bmp_header, 1, sizeof(bmp_header), file_id);

    // Create a temporary buffer
    buffer = malloc(bmp_header.raw_size);
    if (buffer == NULL)
        NF_Error(102, NULL, bmp_header.raw_size);

    // Read data to RAM
    fseek(file_id, bmp_header.offset, SEEK_SET);
    fread(buffer, 1, bmp_header.raw_size, file_id);
    fclose(file_id);

    // Free the previous buffer and allocate a new one
    free(NF_BG16B[slot].buffer);

    u32 size = bmp_header.width * bmp_header.height * 2;
    NF_BG16B[slot].buffer = malloc(size);
    if (NF_BG16B[slot].buffer == NULL)
        NF_Error(102, NULL, size);

    switch (bmp_header.bpp)
    {
        case 8: // 8 bits per pixel
        {
            // Calculate palette size
            u32 colors = (bmp_header.offset - 0x36) / 4; // Stored as RGBA
            char *palette = malloc(colors * 4);
            if (palette == NULL)
                NF_Error(102, NULL, colors * 4);

            // Reopen file and read the palette
            file_id = fopen(filename, "rb");
            if (file_id == NULL)
                NF_Error(101, filename, 0);

            fseek(file_id, 0x36, SEEK_SET);
            fread(palette, 1, colors * 4, file_id);
            fclose(file_id);

            u32 idx = 0;

            // Convert file to 16 bit per pixel
            for (u32 y = 0; y < bmp_header.height; y++)
            {
                for (u32 x = 0; x < bmp_header.width; x++)
                {
                    u32 offset = (((bmp_header.height - 1) - y) * bmp_header.width) + x;

                    u32 pixel = buffer[idx] * 4; // RGBA

                    // Split BGR (not RGB) value
                    u32 b = palette[pixel] >> 3;
                    u32 g = palette[pixel + 1] >> 3;
                    u32 r = palette[pixel + 2] >> 3;

                    // Pack the components as RGB
                    pixel = r | (g << 5) | (b << 10) | BIT(15);
                    NF_BG16B[slot].buffer[offset] = pixel;

                    idx++;
                }

                // Align to 4 bytes at the end of the row
                while ((idx % 4) != 0)
                    idx ++;
            }

            // Free palette from RAM
            free(palette);
            break;
        }
        case 16: // 16 bits per pixel
        {
            u32 idx = 0;

            for (u32 y = 0; y < bmp_header.height; y++)
            {
                for (u32 x = 0; x < bmp_header.width; x++)
                {
                    u32 offset = (((bmp_header.height - 1) - y) * bmp_header.width) + x;

                    u32 pixel = buffer[idx] + (buffer[idx + 1] << 8);

                    // Split BGR (not RGB) value
                    u32 b = pixel & 0x1F;
                    u32 g = (pixel >> 5) & 0x1F;
                    u32 r = (pixel >> 10) & 0x1F;

                    // Pack the components as RGB
                    pixel = r | (g << 5) | (b << 10) | BIT(15);
                    NF_BG16B[slot].buffer[offset] = pixel;

                    idx += 2;
                }

                // Align to 4 bytes at the end of the row
                while ((idx % 4) != 0)
                    idx ++;
            }
            break;
        }
        case 24: // 24 bits per pixel
        {
            u32 idx = 0;

            for (u32 y = 0; y < bmp_header.height; y++)
            {
                for (u32 x = 0; x < bmp_header.width; x++)
                {
                    u32 offset = (((bmp_header.height - 1) - y) * bmp_header.width) + x;

                    u32 b = buffer[idx] >> 3;
                    u32 g = buffer[idx + 1] >> 3;
                    u32 r = buffer[idx + 2] >> 3;

                    // Pack the components as RGB
                    u32 pixel = r | (g << 5) | (b << 10) | BIT(15);
                    NF_BG16B[slot].buffer[offset] = pixel;

                    idx += 3;
                }

                // Align to 4 bytes at the end of the row
                while ((idx % 4) != 0)
                    idx ++;
            }
            break;
        }
        default:
            NF_Error(106, NULL, bmp_header.bpp);
            break;
    }

    // Save background information
    NF_BG16B[slot].size = size;
    NF_BG16B[slot].width = bmp_header.width;
    NF_BG16B[slot].height = bmp_header.height;
    NF_BG16B[slot].inuse = true; // Mark as used

    // Free temporary buffer
    free(buffer);
}
