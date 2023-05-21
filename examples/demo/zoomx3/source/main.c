// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of applying a x3 zoom with interpolation to a 16 bit image.
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
            square_x = touchscreen.px - 42;
            square_y = touchscreen.py - 32;
        }

        // Restrict coordinates of the window
        if (square_x < 0)
            square_x = 0;
        if (square_x > 170)
            square_x = 170;

        if (square_y < 0)
            square_y = 0;
        if (square_y > 128)
            square_y = 128;

        // Zoom read coordinate pointers
        int zoom_x = 0;
        int zoom_y = 0;

        // Fill buffer
        for (int y = square_y; y < (square_y + 64); y++)
        {
            for (int x = square_x; x < (square_x + 85); x++)
            {
                u32 rgb;
                u32 red, green, blue;

                if (zoom_x < 128)
                {
                    // The left part of the screen has bilinear interpolation

                    u32 r[5], g[5], b[5];

                    // Pixel matrix:
                    // 01 A1 A2 02
                    // B1 C1 D1
                    // B2 D2 C2
                    // 03       04

                    // Calculations:
                    // A1 = (1 + 1 + 2) / 3
                    // A2 = (1 + 2 + 2) / 3
                    // B1 = (1 + 1 + 3) / 3
                    // B2 = (1 + 3 + 3) / 3
                    // C1 = (1 + 1 + 4) / 3
                    // C2 = (1 + 4 + 4) / 3
                    // D1 = (2 + 2 + 3) / 3
                    // D2 = (2 + 3 + 3) / 3

                    // 01: Get color of (X, Y)
                    rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];
                    r[1] = rgb & 0x1F;
                    g[1] = (rgb >> 5) & 0x1F;
                    b[1] = (rgb >> 10) & 0x1F;

                    // 02: Get color of (X + 1, Y)
                    if (x < 255)
                        rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + (x + 1)];
                    else
                        rgb = 0;

                    r[2] = rgb & 0x1F;
                    g[2] = (rgb >> 5) & 0x1F;
                    b[2] = (rgb >> 10) & 0x1F;

                    // 03: Get color of (X, Y + 1)
                    if (y < 191)
                        rgb = NF_16BITS_BACKBUFFER[1][((y + 1) << 8) + x];
                    else
                        rgb = 0;

                    r[3] = rgb & 0x1F;
                    g[3] = (rgb >> 5) & 0x1F;
                    b[3] = (rgb >> 10) & 0x1F;

                    // 04: Get color of (X + 1, Y + 1)
                    if ((x < 255) && (y < 191))
                        rgb = NF_16BITS_BACKBUFFER[1][((y + 1) << 8) + (x + 1)];
                    else
                        rgb = 0;

                    r[4] = rgb & 0x1F;
                    g[4] = (rgb >> 5) & 0x1F;
                    b[4] = (rgb >> 10) & 0x1F;

                    // Draw the vertical line that splits both halves
                    if (zoom_x == 126)
                    {
                        for (int i = 1; i < 5; i++)
                        {
                            r[i] = 5;
                            g[i] = 10;
                            b[i] = 15;
                        }
                    }

                    // Calculate value of pixel 01
                    rgb = r[1] | (g[1] << 5) | (b[1] << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + zoom_x)] = rgb;

                    // Calculate value of pixel A1
                    red = (r[1] + r[1] + r[2]) / 3;
                    green = (g[1] + g[1] + g[2]) / 3;
                    blue = (b[1] + b[1] + b[2]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + (zoom_x + 1)] = rgb;

                    // Calculate value of pixel A2
                    red = (r[1] + r[2] + r[2]) / 3;
                    green = (g[1] + g[2] + g[2]) / 3;
                    blue = (b[1] + b[2] + b[2]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + (zoom_x + 2)] = rgb;

                    // Calculate value of pixel B1
                    red = (r[1] + r[1] + r[3]) / 3;
                    green = (g[1] + g[1] + g[3]) / 3;
                    blue = (b[1] + b[1] + b[3]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + zoom_x] = rgb;

                    // Calculate value of pixel B2
                    red = (r[1] + r[3] + r[3]) / 3;
                    green = (g[1] + g[3] + g[3]) / 3;
                    blue = (b[1] + b[3] + b[3]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 2) << 8) + zoom_x] = rgb;

                    // Calculate value of pixel C1
                    red = (r[1] + r[1] + r[4]) / 3;
                    green = (g[1] + g[1] + g[4]) / 3;
                    blue = (b[1] + b[1] + b[4]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + (zoom_x + 1)] = rgb;

                    // Calculate value of pixel C2
                    red = (r[1] + r[4] + r[4]) / 3;
                    green = (g[1] + g[4] + g[4]) / 3;
                    blue = (b[1] + b[4] + b[4]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 2) << 8) + (zoom_x + 2)] = rgb;

                    // Calculate value of pixel D1
                    red = (r[2] + r[2] + r[3]) / 3;
                    green = (g[2] + g[2] + g[3]) / 3;
                    blue = (b[2] + b[2] + b[3]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + (zoom_x + 2)] = rgb;

                    // Calculate value of pixel D2
                    red = (r[2] + r[3] + r[3]) / 3;
                    green = (g[2] + g[3] + g[3]) / 3;
                    blue = (b[2] + b[3] + b[3]) / 3;
                    rgb = red | (green << 5) | (blue << 10) | BIT(15);
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 2) << 8) + (zoom_x + 1)] = rgb;
                }
                else
                {
                    // The right half of the screen uses the nearest pixel

                    rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];

                    // Write pixels in the backbuffer of the top screen.
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + zoom_x] = rgb; // 01
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + (zoom_x + 1)] = rgb; // A1
                    NF_16BITS_BACKBUFFER[0][(zoom_y << 8) + (zoom_x + 2)] = rgb; // A2
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + zoom_x] = rgb; // B1
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 2) << 8) + zoom_x] = rgb; // B2
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + (zoom_x + 1)] = rgb; // C1
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 2) << 8) + (zoom_x + 2)] = rgb; // C2
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 1) << 8) + (zoom_x + 2)] = rgb; // D1
                    NF_16BITS_BACKBUFFER[0][((zoom_y + 2) << 8) + (zoom_x + 1)] = rgb; // D2
                }

                // Get RGB components
                rgb = NF_16BITS_BACKBUFFER[1][(y << 8) + x];
                red = rgb & 0x1F;
                green = (rgb >> 5) & 0x1F;
                blue = (rgb >> 10) & 0x1F;

                // Generate a grayscale value based on the RGB components
                int composite = ((red + green + blue) / 3) + 3;
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
                zoom_x += 3;
            }

            // Set X coordinate from the beginning
            zoom_x = 0;
            // Increment Y coordinate pointer
            zoom_y += 3;
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Swap backbuffers and visible buffers
        NF_Flip16bitsBackBuffer(0);
        NF_Flip16bitsBackBuffer(1);
    }

    return 0;
}
