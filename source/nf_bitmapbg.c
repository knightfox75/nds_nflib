// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Bitmap background functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_2d.h"
#include "nf_basic.h"
#include "nf_bitmapbg.h"

// Structures that hold information about 16 bit bitmap backgrounds
NF_TYPE_BG16B_INFO NF_BG16B[NF_SLOTS_BG16B];

// Backbuffer of 16 bit bitmaps for each screen
u16 *NF_16BITS_BACKBUFFER[2];

// Structures that hold information about 8 bit bitmap backgrounds
NF_TYPE_BG8B_INFO NF_BG8B[NF_SLOTS_BG8B];

// Backbuffer of 8 bit bitmaps for each screen
NF_TYPE_BB8B_INFO NF_8BITS_BACKBUFFER[2];

void NF_Init16bitsBgBuffers(void)
{
    for (int n = 0; n < NF_SLOTS_BG16B; n++)
    {
        NF_BG16B[n].buffer = NULL;
        NF_BG16B[n].size = 0;
        NF_BG16B[n].inuse = false;
        NF_BG16B[n].width = 0;
        NF_BG16B[n].height = 0;
    }
}

void NF_Reset16bitsBgBuffers(void)
{
    // Free buffers
    for (int n = 0; n < NF_SLOTS_BG16B; n++)
        free(NF_BG16B[n].buffer);

    // Reset background information
    NF_Init16bitsBgBuffers();
}

void NF_Init16bitsBackBuffer(int screen)
{
    if (screen > 1)
        screen = 1;

    NF_16BITS_BACKBUFFER[screen] = NULL;
}

void NF_Enable16bitsBackBuffer(int screen)
{
    if (screen > 1)
        screen = 1;

    // Free buffer if it was already allocated
    free(NF_16BITS_BACKBUFFER[screen]);
    NF_16BITS_BACKBUFFER[screen] = NULL;

    // Try to allocate 128 KB
    NF_16BITS_BACKBUFFER[screen] = calloc(65536, sizeof(u16));

    // Fail if there isn't enough free memory
    if (NF_16BITS_BACKBUFFER[screen] == NULL)
        NF_Error(102, NULL, 131072);
}

void NF_Disble16bitsBackBuffer(int screen)
{
    if (screen > 1)
        screen = 1;

    // Free buffer if it was already allocated
    free(NF_16BITS_BACKBUFFER[screen]);
    NF_16BITS_BACKBUFFER[screen] = NULL;
}

void NF_Flip16bitsBackBuffer(int screen)
{
    // Copy contents of the backuffer to VRAM
    if (screen == 0)
        NF_DmaMemCopy((void *)0x06000000, NF_16BITS_BACKBUFFER[0], 131072);
    else
        NF_DmaMemCopy((void *)0x06200000, NF_16BITS_BACKBUFFER[1], 131072);
}

void NF_InitBitmapBgSys(int screen, u32 mode)
{
    // Setup layer 3 (and optionally layer 2) of the selected screen as a bitmap

    // Initialize VRAM
    if (screen == 0)
    {
        vramSetBankA(VRAM_A_MAIN_BG);           // VRAM_A: Main engine backgrounds (128 KB)
        memset((void *)0x06000000, 0, 131072);  // Clear VRAM_A
        for (int n = 0; n < 4; n++)             // Hide all 4 layers
            NF_HideBg(0, n);
    }
    else
    {
        vramSetBankC(VRAM_C_SUB_BG);            // VRAM_C: Sub engine backgrounds (128 KB)
        memset((void*)0x06200000, 0, 131072);   // Clear VRAM_C
        for (int n = 0; n < 4; n++)             // Hide all 4 layers
            NF_HideBg(1, n);
    }

    // Initialize drawing layer
    if (screen == 0)
    {
        if (mode == 0)
        {
            // Setup layers 2 and 3 in 8 bit mode
            REG_BG3CNT = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
            REG_BG2CNT = BG_PRIORITY_2 | BG_BMP_BASE(0) | BG_BMP8_256x256;
        }
        else
        {
            // Setup layer 3 in 16 bit mode
            REG_BG3CNT = BG_PRIORITY_3 | BG_BMP_BASE(0) | BG_BMP16_256x256;
        }

        // Reset rotation/scaling of layer 3
        REG_BG3PA = 1 << 8;
        REG_BG3PB = 0;
        REG_BG3PC = 0;
        REG_BG3PD = 1 << 8;
        NF_ScrollBg(0, 3, 0, 0); // Reset scroll
        NF_ShowBg(0, 3);         // Show layer 3

        if (mode == 0)
        {
            // Reset rotation/scaling of layer 2
            REG_BG2PA = 1 << 8;
            REG_BG2PB = 0;
            REG_BG2PC = 0;
            REG_BG2PD = 1 << 8;
            NF_ScrollBg(0, 2, 0, 0); // Reset scroll
            NF_ShowBg(0, 2);         // Show layer 2
        }
    }
    else
    {
        if (mode == 0)
        {
            // Setup layers 2 and 3 in 8 bit mode
            REG_BG3CNT_SUB = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
            REG_BG2CNT_SUB = BG_PRIORITY_2 | BG_BMP_BASE(0) | BG_BMP8_256x256;
        }
        else
        {
            // Setup layer 3 in 16 bit mode
            REG_BG3CNT_SUB = BG_PRIORITY_3 | BG_BMP_BASE(0) | BG_BMP16_256x256;
        }

        // Reset rotation/scaling of layer 3
        REG_BG3PA_SUB = 1 << 8;
        REG_BG3PB_SUB = 0;
        REG_BG3PC_SUB = 0;
        REG_BG3PD_SUB = 1 << 8;
        NF_ScrollBg(1, 3, 0, 0); // Reset scroll
        NF_ShowBg(1, 3);         // Show layer 3

        if (mode == 0)
        {
            // Reset rotation/scaling of layer 2
            REG_BG2PA_SUB = 1 << 8;
            REG_BG2PB_SUB = 0;
            REG_BG2PC_SUB = 0;
            REG_BG2PD_SUB = 1 << 8;
            NF_ScrollBg(1, 2, 0, 0); // Reset scroll
            NF_ShowBg(1, 2);         // Show layer 2
        }
    }
}

void NF_Load16bitsBg(const char *file, u32 slot)
{
    NF_Load16bImgData(file, slot, 256, 256, 0);
}

void NF_Load16bitsImage(const char *file, u32 slot, u32 size_x, u32 size_y)
{
    NF_Load16bImgData(file, slot, size_x, size_y, 1);
}

void NF_Load16bImgData(const char *file, u32 slot, u32 x, u32 y, u32 type)
{
    // Verify that the slot is in the valid range
    if (slot >= NF_SLOTS_BG16B)
    {
        if (type == 0)
            NF_Error(106, "16 bit BG", NF_SLOTS_BG16B);
        else
            NF_Error(106, "16 bit image", NF_SLOTS_BG16B);
    }

    // Free buffer if it is already in use
    free(NF_BG16B[slot].buffer);
    NF_BG16B[slot].buffer = NULL;

    // Load .IMG file
    char filename[256];
    char *buffer;
    size_t size;
    snprintf(filename, sizeof(filename), "%s/%s.img", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &buffer, &size, 0);

    // If the size is too big (over 128 KB), fail
    if (size > 131072)
        NF_Error(116, filename, 131072);

    NF_BG16B[slot].buffer = (void *)buffer;

    // Ensure that the alpha bit is set to 1
    for (u32 n = 0; n < (size / 2); n++)
        NF_BG16B[slot].buffer[n] |= BIT(15);

    // Save background parameters
    NF_BG16B[slot].size = size;
    NF_BG16B[slot].width = x;
    NF_BG16B[slot].height = y;
    NF_BG16B[slot].inuse = true; // Set slot as being used
}

void NF_Unload16bitsBg(u32 slot)
{
    // Verify that the slot contains data
    if (!NF_BG16B[slot].inuse)
        NF_Error(110, "16 bit BG", slot);

    // Free the buffer
    free(NF_BG16B[slot].buffer);
    NF_BG16B[slot].buffer = NULL;

    NF_BG16B[slot].size = 0;
    NF_BG16B[slot].inuse = false; // Mark slot as being free
}

void NF_Copy16bitsBuffer(int screen, int destination, u32 slot)
{
    // Verify that the slot contains data
    if (!NF_BG16B[slot].inuse)
        NF_Error(110, "16 bit BG", slot);

    if (destination == 0) // Destination is VRAM
    {
        if (screen == 0)
            NF_DmaMemCopy((void *)0x06000000, NF_BG16B[slot].buffer, NF_BG16B[slot].size);
        else
            NF_DmaMemCopy((void *)0x06200000, NF_BG16B[slot].buffer, NF_BG16B[slot].size);
    }
    else // Destination is backbuffer
    {
        if (screen == 0)
            memcpy(NF_16BITS_BACKBUFFER[0], NF_BG16B[slot].buffer, NF_BG16B[slot].size);
        else
            memcpy(NF_16BITS_BACKBUFFER[1], NF_BG16B[slot].buffer, NF_BG16B[slot].size);
    }
}

void NF_Draw16bitsImage(int screen, u32 slot, s32 x, s32 y, bool alpha)
{
    // Verify that the slot contains data
    if (!NF_BG16B[slot].inuse)
        NF_Error(110, "16 Bits Image", slot);

    u8 scr = screen;
    if (scr > 1)
        scr = 1;

    // The destination is the backbuffer
    for (int img_y = 0; img_y < NF_BG16B[slot].height; img_y++)
    {
        for (int img_x = 0; img_x < NF_BG16B[slot].width; img_x++)
        {
            // Location of the destination pixel
            int buff_x = img_x + x;
            int buff_y = img_y + y;

            // Draw it if it's inside the background
            if ((buff_x >= 0) && (buff_x <= 255) && (buff_y >= 0) && (buff_y <= 255))
            {
                u32 buff_idx = (buff_y << 8) + buff_x;
                u32 data = NF_BG16B[slot].buffer[(img_y * NF_BG16B[slot].width) + img_x];

                // Write pixel if it isn't magenta (RGB15(31, 0, 31) | BIT(15))
                if ((data != 0xFC1F) || (!alpha))
                    *(NF_16BITS_BACKBUFFER[scr] + buff_idx) = data;
            }
        }
    }
}

void NF_Init8bitsBgBuffers(void)
{
    for (int n = 0; n < NF_SLOTS_BG8B; n++)
    {
        NF_BG8B[n].data = NULL;
        NF_BG8B[n].pal = NULL;
        NF_BG8B[n].data_size = 0;
        NF_BG8B[n].pal_size = 0;
        NF_BG8B[n].inuse = false;
    }
}

void NF_Reset8bitsBgBuffers(void)
{
    // Free the RAM buffers
    for (int n = 0; n < NF_SLOTS_BG8B; n++)
    {
        free(NF_BG8B[n].data);
        free(NF_BG8B[n].pal);
    }

    // Reset data structures
    NF_Init8bitsBgBuffers();
}

void NF_Load8bitsBg(const char *file, u32 slot)
{
    // Verify that the slot is in the valid range
    if (slot >= NF_SLOTS_BG8B)
        NF_Error(106, "8 Bits Bg's", NF_SLOTS_BG8B);

    // Free buffers if they are already in use
    free(NF_BG8B[slot].data);
    NF_BG8B[slot].data = NULL;
    free(NF_BG8B[slot].pal);
    NF_BG8B[slot].pal = NULL;

    // File path
    char filename[256];

    // Load .IMG file
    size_t size;
    char *buffer;
    snprintf(filename, sizeof(filename), "%s/%s.img", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &buffer, &size, 0);

    // If it's too big (more than 64 KB), fail
    if (size > 65536)
        NF_Error(116, filename, 65536);

    NF_BG8B[slot].data = (void *)buffer;
    NF_BG8B[slot].data_size = size;

    // Load .PAL file
    snprintf(filename, sizeof(filename), "%s/%s.pal", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &buffer, &NF_BG8B[slot].pal_size, 256 * 2);

    NF_BG8B[slot].pal = (void *)buffer;

    // Mark this slot as being in use
    NF_BG8B[slot].inuse = true;
}

void NF_Unload8bitsBg(u32 slot)
{
    // Verify that the slot contains data
    if (!NF_BG8B[slot].inuse)
        NF_Error(110, "8 Bits Bg", slot);

    // Free buffers
    free(NF_BG8B[slot].data);
    NF_BG8B[slot].data = NULL;
    NF_BG8B[slot].data_size = 0;
    free(NF_BG8B[slot].pal);
    NF_BG8B[slot].pal = NULL;
    NF_BG8B[slot].pal_size = 0;

    NF_BG8B[slot].inuse = false; // Mark slot as free
}

void NF_Copy8bitsBuffer(int screen, int destination, u32 slot)
{
    // Verify that the slot contains data
    if (!NF_BG8B[slot].inuse) NF_Error(110, "8 Bits Bg", slot);

    if (screen > 1)
        screen = 1;

    // Si el destino es la VRAM
    if (destination < 2)
    {
        u32 data, pal; // Addresses in VRAM for data and palette

        if (screen == 0)
        {
            data = 0x06000000;
            pal = 0x05000000;
        }
        else
        {
            data = 0x06200000;
            pal = 0x05000400;
        }

        if (destination == 1)
            data += 65536;

        // Copy data to VRAM
        NF_DmaMemCopy((void *)data, NF_BG8B[slot].data, NF_BG8B[slot].data_size);
        NF_DmaMemCopy((void *)pal, NF_BG8B[slot].pal, NF_BG8B[slot].pal_size);
    }
    else
    {
        // Copy data to backbuffer
        memcpy(NF_8BITS_BACKBUFFER[screen].data, NF_BG8B[slot].data,
               NF_BG8B[slot].data_size);
        memcpy(NF_8BITS_BACKBUFFER[screen].pal, NF_BG8B[slot].pal,
               NF_BG8B[slot].pal_size);
    }
}

void NF_Init8bitsBackBuffer(int screen)
{
    if (screen > 1)
        screen = 1;

    NF_8BITS_BACKBUFFER[screen].data = NULL;
    NF_8BITS_BACKBUFFER[screen].pal = NULL;
}

void NF_Enable8bitsBackBuffer(int screen)
{
    if (screen > 1)
        screen = 1;

    // Free buffers
    free(NF_8BITS_BACKBUFFER[screen].data);
    free(NF_8BITS_BACKBUFFER[screen].pal);
    NF_8BITS_BACKBUFFER[screen].data = NULL;
    NF_8BITS_BACKBUFFER[screen].pal = NULL;

    // Allocate 64 KB for bitmap data
    NF_8BITS_BACKBUFFER[screen].data = calloc(65536, sizeof(u8));
    if (NF_8BITS_BACKBUFFER[screen].data == NULL)
        NF_Error(102, NULL, 65536);

    // Allocate 512 bytes for bitmap palette
    NF_8BITS_BACKBUFFER[screen].pal = calloc(256, sizeof(u16));
    if (NF_8BITS_BACKBUFFER[screen].pal == NULL)
        NF_Error(102, NULL, 512);
}

void NF_Disble8bitsBackBuffer(int screen)
{
    if (screen > 1)
        screen = 1;

    // Free buffers
    free(NF_8BITS_BACKBUFFER[screen].data);
    free(NF_8BITS_BACKBUFFER[screen].pal);
    NF_8BITS_BACKBUFFER[screen].data = NULL;
    NF_8BITS_BACKBUFFER[screen].pal = NULL;
}

void NF_Flip8bitsBackBuffer(int screen, int destination)
{
    // Copy contents of the backuffer to VRAM

    if (screen > 1)
        screen = 1;

    u32 data, pal; // Addresses in VRAM for data and palette

    if (screen == 0)
    {
        data = 0x06000000;
        pal = 0x05000000;
    }
    else
    {
        data = 0x06200000;
        pal = 0x05000400;
    }

    if (destination == 1)
        data += 65536;

    NF_DmaMemCopy((void *)data, NF_8BITS_BACKBUFFER[screen].data, 65536);
    NF_DmaMemCopy((void *)pal, NF_8BITS_BACKBUFFER[screen].pal, 512);
}
