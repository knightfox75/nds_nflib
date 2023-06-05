// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Simple wave effect
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

// Variables
u16 mapa[32][24];   // Holds the map
s16 bgx[192];       // Horizontal scroll of each line
s8 i[192];          // Scroll speed of each line

// Function that runs after a scanline is drawn. By modifying the values of the
// scroll registers it's possible to add a wave effect.
void hblank_handler(void)
{
    u32 vline = REG_VCOUNT; // Get the current line

    if (vline < 192)
    {
        // If this is a line inside the screen, handle the effect
        bgx[vline] += i[vline];

        if ((bgx[vline] < 1) || (bgx[vline] > 63))
            i[vline] *= -1;

        NF_ScrollBg(1, 3, (bgx[vline] / 8) - 4, 0);
    }
}

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

    // Initialize 2D engine in both screens and use mode 0
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "nfl");

    // Create bottom screen background
    NF_CreateTiledBg(1, 3, "nfl");

    // Flip vertically all the tiles in the map of the bottom screen, and copy
    // them to a temporary buffer
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            NF_SetTileVflip(1, 3, x, y);
            mapa[x][y] = NF_GetTileOfMap(1, 3, x, y);
        }
    }

    // Now copy the tiles in reverse order to flip the image correctly
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            NF_SetTileOfMap(1, 3, x, (23 - y), mapa[x][y]);
        }
    }

    // Finally, update the copy of the map in VRAM
    NF_UpdateVramMap(1, 3);

    // Initialize the wave effect
    s8 inc = 1;
    int x = 0;

    for (int y = 0; y < 192; y++)
    {
        x += inc;
        bgx[y] = x;

        if ((x < 1) || (x > 63))
            inc *= -1;

        i[y] = inc;
    }

    // Register a function to be called after a screen line has been drawn (when
    // the horizontal blanking period starts)
    irqSet(IRQ_HBLANK, hblank_handler);
    irqEnable(IRQ_HBLANK);

    while (1)
    {
        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
