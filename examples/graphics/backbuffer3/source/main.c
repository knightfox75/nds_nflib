// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Basic example of using a 16-bit bitmap with a backbuffer and loading images.
// It shows two images with a window effect.
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
    NF_Load16bitsBg("bmp/img16_a", 1);
    NF_Load16bitsBg("bmp/img16_b", 2);

    // Tranfer image to VRAM of both screens
    NF_Copy16bitsBuffer(0, 0, 0);
    NF_Copy16bitsBuffer(1, 1, 2);

    // It's not needed to use it any longer, so remove it from RAM
    NF_Unload16bitsBg(0);

    // Variables to control the window
    s16 xa = 0;
    s16 xb = 0;
    s16 ya = 0;
    s16 yb = 0;

    while (1)
    {
        // Copy original image to the backbuffer of the top screen
        NF_Copy16bitsBuffer(1, 1, 2);

        // Read keys and touchscreen
        scanKeys();
        touchPosition touchscreen;
        touchRead(&touchscreen);
        u16 keys_held = keysHeld();
        u16 keys_down = keysDown();

        if (keys_down & KEY_TOUCH)
        {
            // If this is the first time the user presses the screen, save that
            // point as one corner of the window.
            xa = touchscreen.px;
            ya = touchscreen.py;
        }
        else if (keys_held & KEY_TOUCH)
        {
            // Save the other corner of the window
            xb = touchscreen.px;
            yb = touchscreen.py;

            // If the two points aren't the same
            if ((xa != xb) && (ya != yb))
            {
                int sqr_xa, sqr_xb, sqr_ya, sqr_yb;

                // Calculate window bounds
                if (xa < xb)
                {
                    sqr_xa = xa;
                    sqr_xb = xb;
                }
                else
                {
                    sqr_xa = xb;
                    sqr_xb = xa;
                }

                if (ya < yb)
                {
                    sqr_ya = ya;
                    sqr_yb = yb;
                }
                else
                {
                    sqr_ya = yb;
                    sqr_yb = ya;
                }

                // Draw the window
                for (int y = sqr_ya; y < sqr_yb; y++)
                {
                    u32 offset = (y << 8) + sqr_xa;

                    memcpy(NF_16BITS_BACKBUFFER[1] + offset,
                           NF_BG16B[1].buffer + offset,
                           (sqr_xb - sqr_xa) * 2);
                }
            }
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Swap backbuffer and visible buffers
        NF_Flip16bitsBackBuffer(1);
    }

    return 0;
}
