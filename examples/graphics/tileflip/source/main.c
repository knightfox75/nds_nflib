// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of flipping background tiles horizontally and verticaly.
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

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "nfl");

    // Temporary map storage (32x24 tiles)
    u16 mapa[32][24];

    // Invert tiles of the bottom screen and write it to the temporary buffer
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            NF_SetTileHflip(1, 3, x, y);
            NF_SetTileVflip(1, 3, x, y); // Comment out for mirror effect
            mapa[x][y] = NF_GetTileOfMap(1, 3, x, y);
        }
    }

    // Place tiles in inverse order
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            // Comment out for mirror effect
            NF_SetTileOfMap(1, 3, 31 - x, 23 - y, mapa[x][y]);

            // Uncomment for mirror effect
            //NF_SetTileOfMap(1, 3, 31 - x, y, mapa[x][y]);
        }
    }

    // Update copy of the map in VRAM with the copy in RAM
    NF_UpdateVramMap(1, 3);

    while (1)
    {
        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
