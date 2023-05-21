// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Basic example of using a 16-bit bitmap with a backbuffer and loading images.
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

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

    // Initialize backbuffer
    NF_Init16bitsBackBuffer(1);

    // Enable backbuffer
    NF_Enable16bitsBackBuffer(1);

    // Load bitmap background files from NitroFS
    NF_Load16bitsBg("bmp/bitmap16", 0);

    // Tranfer image to VRAM of the top screen
    NF_Copy16bitsBuffer(0, 0, 0);
    // Tranfer image to the backbuffer of the bottom screen
    NF_Copy16bitsBuffer(1, 1, 0);

    // Window location
    s16 square_x = 0;
    s16 square_y = 0;

    while (1)
    {
        // Copy original image
        NF_Copy16bitsBuffer(1, 1, 0);

        // Read keys and touchscreen
        scanKeys();
        touchPosition touchscreen;
        touchRead(&touchscreen);
        u16 keys = keysHeld();
        if (keys & KEY_TOUCH)
        {
            square_x = touchscreen.px - 32;
            square_y = touchscreen.py - 32;
        }

        // Restrict coordinates of the window
        if (square_x < 0)
            square_x = 0;
        if (square_x > 191)
            square_x = 191;

        if (square_y < 0)
            square_y = 0;
        if (square_y > 127)
            square_y = 127;

        // Fill buffer
        for (int y = square_y; y < square_y + 64; y++)
        {
            for (int x = square_x; x < square_x + 64; x++)
            {
                // Get current color
                u32 rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];
                u32 r = rgb & 0x1F;
                u32 g = (rgb >> 5) & 0x1F;
                u32 b = (rgb >> 10) & 0x1F;

                // Generate a grayscale value based on the RGB components
                u32 composite = ((r + g + b) / 3) + 3;
                if (composite > 31)
                    composite = 31;

                // Replace the RGB values by the grayscale value (this shows up
                // as a rectangular window on the screen)
                r = composite;
                g = composite;
                b = composite;

                // Pack the components as a RGB value
                rgb = r | (g << 5) | (b << 10) | BIT(15);

                // Write RGB value in the backbuffer
                NF_16BITS_BACKBUFFER[1][(y << 8) + x] = rgb;
            }
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Swap backbuffer and visible buffers
        NF_Flip16bitsBackBuffer(1);
    }

    return 0;
}
