// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of applying a x2 zoom with interpolation to a 16 bit image.
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

    // Initialize backbuffers
    NF_Init16bitsBackBuffer(0);
    NF_Init16bitsBackBuffer(1);

    // Enable backbuffers
    NF_Enable16bitsBackBuffer(0);
    NF_Enable16bitsBackBuffer(1);

    // Load bitmap files from NitroFS
    NF_Load16bitsBg("bmp/img16_c", 0);

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
            square_x = touchscreen.px - 64;
            square_y = touchscreen.py - 48;
        }

        // Restrict coordinates of the window
        if (square_x < 0)
            square_x = 0;
        if (square_x > 127)
            square_x = 127;

        if (square_y < 0)
            square_y = 0;
        if (square_y > 95)
            square_y = 95;

        // Zoom read coordinate pointers
        int zoom_x = 0;
        int zoom_y = 0;

        // Fill buffer
        for (int y = square_y; y < square_y + 96; y++)
        {
            for (int x = square_x; x < square_x + 128; x++)
            {
                u32 rgb;
                u32 red, green, blue;

                if (zoom_x < 128)
                {
                    // The left part of the screen has bilinear interpolation

                    u32 r[4], g[4], b[4];

                    // Color matrix:
                    // 0 1
                    // 2 3

                    // 0: Get color of (X, Y)
                    rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];
                    r[0] = rgb & 0x1F;
                    g[0] = (rgb >> 5) & 0x1F;
                    b[0] = (rgb >> 10) & 0x1F;

                    // 1: Get color of (X + 1, Y)
                    if (x < 255)
                        rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + (x + 1)];
                    else
                        rgb = 0;

                    r[1] = rgb & 0x1F;
                    g[1] = (rgb >> 5) & 0x1F;
                    b[1] = (rgb >> 10) & 0x1F;

                    // 2: Get color of (X, Y + 1)
                    if (y < 191)
                        rgb = NF_16BITS_BACKBUFFER[1][((y + 1) << 8) + x];
                    else
                        rgb = 0;

                    r[2] = rgb & 0x1F;
                    g[2] = (rgb >> 5) & 0x1F;
                    b[2] = (rgb >> 10) & 0x1F;

                    // 3: Get color of (X + 1, Y + 1)
                    if ((x < 255) && (y < 191))
                        rgb = NF_16BITS_BACKBUFFER[1][((y + 1) << 8) + (x + 1)];
                    else
                        rgb = 0;

                    r[3] = rgb & 0x1F;
                    g[3] = (rgb >> 5) & 0x1F;
                    b[3] = (rgb >> 10) & 0x1F;

                    // Draw the vertical line that splits both halves
                    if (zoom_x == 126)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            r[i] = 5;
                            g[i] = 10;
                            b[i] = 15;
                        }
                    }

                    // Calculate value of (X, Y)
                    rgb = r[0] | (g[0] << 5) | (b[0] << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + zoom_x] = rgb;

                    // Calculate value of (X + 1, Y)
                    red = (r[0] + r[1]) >> 1;
                    green = (g[0] + g[1]) >> 1;
                    blue = (b[0] + b[1]) >> 1;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + (zoom_x + 1)] = rgb;

                    // Calculate value of (X, Y + 1)
                    red = (r[0] + r[2]) >> 1;
                    green = (g[0] + g[2]) >> 1;
                    blue = (b[0] + b[2]) >> 1;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + zoom_x] = rgb;

                    // Calculate value of (X + 1, Y + 1)
                    red = ((r[0] + r[3]) >> 1);
                    green = ((g[0] + g[3]) >> 1);
                    blue = ((b[0] + b[3]) >> 1);
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + (zoom_x + 1)] = rgb;
                }
                else
                {
                    // The right half of the screen uses the nearest pixel

                    rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];

                    // Write pixels in the backbuffer of the top screen.
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + zoom_x] = rgb;
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + (zoom_x + 1)] = rgb;
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + zoom_x] = rgb;
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + (zoom_x + 1)] = rgb;
                }

                // Get RGB components
                rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];
                red = rgb & 0x1F;
                green = (rgb >> 5) & 0x1F;
                blue = (rgb >> 10) & 0x1F;

                // Generate a grayscale value based on the RGB components
                u32 composite = ((red + green + blue) / 3) + 3;
                if (composite > 31)
                    composite = 31;

                // Replace the RGB values by the grayscale value (this shows up
                // as a rectangular window on the screen)
                red = composite >> 1;
                green = composite >> 2;
                blue = composite;

                // Pack the components as a RGB value
                rgb = red | (green << 5) | (blue << 10) | BIT(15);

                // Write RGB value in the backbuffer
                NF_16BITS_BACKBUFFER[1][(y << 8) + x] = rgb;

                // Increment X coordinate pointer
                zoom_x += 2;
            }

            // Set X coordinate from the beginning
            zoom_x = 0;
            // Increment Y coordinate pointer
            zoom_y += 2;
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Swap backbuffers and visible buffers
        NF_Flip16bitsBackBuffer(0);
        NF_Flip16bitsBackBuffer(1);
    }

    return 0;
}
