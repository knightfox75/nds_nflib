// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of loading BMP files and scrolling them.
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define SPEED 3

int main(int argc, char **argv)
{
    // Prepare a NitroFS initialization screen
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    consoleDemoInit();
    printf("\n NitroFS init. Please wait.\n\n");
    printf(" Iniciando NitroFS,\n por favor, espere.\n\n");
    swiWaitForVBlank();

    // Initialize NitroFS and set it as the root folder of the filesystem
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // Initialize 2D engine in both screens and use mode 5
    NF_Set2D(0, 5);
    NF_Set2D(1, 5);

    // Initialize bitmap backgrounds system
    NF_InitBitmapBgSys(0, 1);
    NF_InitBitmapBgSys(1, 1);

    // Initialize storage buffers
    NF_Init16bitsBgBuffers();

    // Initialize backbuffers
    NF_Init16bitsBackBuffer(0);
    NF_Init16bitsBackBuffer(1);

    // Enable backbuffers
    NF_Enable16bitsBackBuffer(0);
    NF_Enable16bitsBackBuffer(1);

    // Load bitmap files from NitroFS
    NF_LoadBMP("bmp/lostend", 0);

    // Variables
    s16 x = 0;
    s16 y = 0;
    s16 right = 0;
    s16 bottom = 0;
    u16 mini_x = 0;
    u16 mini_y = 0;
    u16 disp_x = 0;
    u16 disp_y = 0;
    u32 scr_idx = 0;
    u32 img_idx = 0;

    // Calculate drawing canvas size
    if (NF_BG16B[0].width > 256)
        right = 256;
    else
        right = NF_BG16B[0].width;

    if (NF_BG16B[0].height > 192)
        bottom = 192;
    else
        bottom = NF_BG16B[0].height;

    // Calculate thumbnail size and offsets to place on the screen
    if (NF_BG16B[0].width >= NF_BG16B[0].height)
    {
        mini_x = right;
        mini_y = (NF_BG16B[0].height * right) / NF_BG16B[0].width;
        disp_y = (192 - mini_y) / 2;
    }
    else
    {
        mini_x = (NF_BG16B[0].width * bottom) / NF_BG16B[0].height;
        mini_y = bottom;
        disp_x = (256 - mini_x) / 2;
    }

    // Generate thumbnail
    for (int scr_y = 0; scr_y < mini_y; scr_y++)
    {
        for (int scr_x = 0; scr_x < mini_x; scr_x++)
        {
            // Calculate source and destination offsets
            scr_idx = ((scr_y + disp_y) << 8) + (scr_x + disp_x);
            img_idx = (((scr_y * NF_BG16B[0].height) / mini_y) * NF_BG16B[0].width)
                    + ((scr_x * NF_BG16B[0].width) / mini_x);

            *(NF_16BITS_BACKBUFFER[1] + scr_idx) = NF_BG16B[0].buffer[img_idx];
        }
    }

    // Send backbuffer to screen
    NF_Flip16bitsBackBuffer(1);

    while (1)
    {
        scanKeys(); // Read keypad
        uint16_t keys = keysHeld(); // Keys currently pressed

        if (NF_BG16B[0].width > 256)
        {
            if (keys & KEY_LEFT)
            {
                x -= SPEED;
                if (x < 0)
                    x = 0;
            }
            if (keys & KEY_RIGHT)
            {
                x += SPEED;
                if (x > (NF_BG16B[0].width - 256))
                    x = NF_BG16B[0].width - 256;
            }
        }
        if (NF_BG16B[0].height > 192)
        {
            if (keys & KEY_UP)
            {
                y -= SPEED;
                if (y < 0)
                    y = 0;
            }
            if (keys & KEY_DOWN)
            {
                y += SPEED;
                if (y > (NF_BG16B[0].height - 192))
                    y = NF_BG16B[0].height - 192;
            }
        }

        // Draw image on the framebuffer
        for (int scr_y = 0; scr_y < bottom; scr_y++)
        {
            for (int scr_x = 0; scr_x < right; scr_x++)
            {
                // Calculate source and destination offsets
                scr_idx = (scr_y << 8) + scr_x;
                img_idx = ((scr_y + y) * NF_BG16B[0].width) + (scr_x + x);

                *(NF_16BITS_BACKBUFFER[0] + scr_idx) = NF_BG16B[0].buffer[img_idx];
            }
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Send backbuffer to the screen
        NF_Flip16bitsBackBuffer(0);
    }

    return 0;
}
