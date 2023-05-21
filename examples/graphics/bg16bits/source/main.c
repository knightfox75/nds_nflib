// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of using 16-bit bitmap graphics.
// http://www.nightfoxandco.com

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

    u32 r = 0;
    u32 g = 0;
    u32 b = 0;

    // Rellena la pantalla
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
                if (g > 31)
                {
                    g = 0;
                    b++;
                    if (b > 31)
                        b = 0;
                }
            }

            // Pack the components as a RGB value
            u32 rgb = r | (g << 5) | (b << 10) | BIT(15);

            // Calculate destination address for top screen (VRAM_A)
            u32 address = 0x06000000 + (((y << 8) + x) << 1);

            // Write RGB value
            *((u16*)address) = rgb;

            // Calculate destination address for bottom screen (VRAM_C)
            address = 0x06200000 + (((x << 8) + y) << 1);

            // Write RGB value
            *((u16*)address) = rgb;
        }
    }

    while (1)
    {
        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
