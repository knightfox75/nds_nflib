// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// 16-bit bitmap backbuffer example.
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>

#include <nf_lib.h>

int main(int argc, char **argv)
{
    // Initialize 2D engine in both screens and use mode 5
    NF_Set2D(0, 5);
    NF_Set2D(1, 5);

    // Initialize bitmap backgrounds system
    NF_InitBitmapBgSys(0, 1);
    NF_InitBitmapBgSys(1, 1);

    // Initialize backbuffers
    NF_Init16bitsBackBuffer(0);
    NF_Init16bitsBackBuffer(1);

    // Enable backbuffers
    NF_Enable16bitsBackBuffer(0);
    NF_Enable16bitsBackBuffer(1);

    u32 r = 0;
    u32 g = 0;
    u32 b = 0;

    while (1)
    {
        // Fill buffer
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                // Calculate new color
                r++;
                if (r > 31)
                {
                    r = 0;
                    g++;
                    if (g > 16)
                        g = 0;
                }
                b = (y << 5) >> 8;

                // Compose RGB value
                u32 rgb = r | (g << 5) | (b << 10) | BIT(15);

                // Write color to the backbuffer
                NF_16BITS_BACKBUFFER[0][(y << 8) + x] = rgb;
                NF_16BITS_BACKBUFFER[1][(x << 8) + y] = rgb;
            }
        }

        // Send backbuffers to the screen
        NF_Flip16bitsBackBuffer(0);
        NF_Flip16bitsBackBuffer(1);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
