// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of using a 8-bit bitmap backbuffer.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <string.h>

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
    NF_InitBitmapBgSys(0, 0);
    NF_InitBitmapBgSys(1, 0);

    // Initialize storage buffers
    NF_Init8bitsBgBuffers();

    // Initialize backbuffers
    NF_Init8bitsBackBuffer(0);
    NF_Init8bitsBackBuffer(1);

    // Enable backbuffers
    NF_Enable8bitsBackBuffer(0);
    NF_Enable8bitsBackBuffer(1);

    // Load bitmap files from NitroFS
    NF_Load8bitsBg("bmp/img8b_1", 0);
    NF_Load8bitsBg("bmp/img8b_2", 1);
    NF_Load8bitsBg("bmp/img8b_3", 2);

    // Transfer image to the backbuffer of the top screen
    NF_Copy8bitsBuffer(0, 2, 1);
    // Transfer image to layer 3 of the top screen
    NF_Copy8bitsBuffer(0, 1, 0);
    // Transfer image to layer 3 of the bottom screen
    NF_Copy8bitsBuffer(1, 1, 2);

    // Copy palette of the bottom screen to the palette of the backbuffer
    for (int n = 0; n < 256; n++)
        NF_8BITS_BACKBUFFER[1].pal[n] = *((u16*)(0x05000400 + (n * 2)));

    // Set last color of the palette to red to draw using read
    NF_8BITS_BACKBUFFER[1].pal[255] = RGB15(31, 0, 0);

    bool show = true;

    while (1)
    {
        // Read keys and touchscreen
        scanKeys();
        touchPosition touchscreen;
        touchRead(&touchscreen);
        u16 keys_held = keysHeld();
        u16 keys_down = keysDown();

        // Check if the touch screen is pressed
        if (keys_held & KEY_TOUCH)
        {
            int x = touchscreen.px;
            if (x > 247)
                x = 247;

            int y = touchscreen.py;
            if (y > 183)
                y = 183;

            // Draw a 8x8 block in both screens
            for (int dy = 0; dy < 9; dy++)
            {
                for (int dx = 0; dx < 9; dx++)
                {
                    u32 idx = ((y + dy) << 8) + (x + dx);

                    NF_8BITS_BACKBUFFER[0].data[idx] = 0; // Top screen
                    NF_8BITS_BACKBUFFER[1].data[idx] = 255; // Bottom screen
                }
            }
        }

        // Press A to hide or show layer 3 of the bottom screen
        if (keys_down & KEY_A)
        {
            show = !show;
            if (show)
                NF_ShowBg(1, 3);
            else
                NF_HideBg(1, 3);
        }

        // Press X to restore the image in the top screen
        if (keys_down & KEY_X)
            NF_Copy8bitsBuffer(0, 2, 1);

        // Press B to delete the drawing layer
        if (keys_down & KEY_B)
            memset(NF_8BITS_BACKBUFFER[1].data, 0, 65536);

        // Copy backbuffer of the top screen to layer 2
        NF_Flip8bitsBackBuffer(0, 0);

        // Copy backbuffer of the bottom screen to layer 2
        NF_Flip8bitsBackBuffer(1, 0);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
