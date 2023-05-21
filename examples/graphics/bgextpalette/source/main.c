// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of extended palettes for backgrounds.
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define RED 1
#define GREEN 2
#define BLUE 3

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

    // Initialize text system
    NF_InitTextSys(0);          // Top screen
    NF_InitTextSys(1);          // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/blue", "top", 256, 256);
    NF_LoadTiledBg("bg/blue", "bottom", 256, 256);

    // Load extended background palette files from NitroFS
    NF_LoadExBgPal("bg/red", RED);
    NF_LoadExBgPal("bg/green", GREEN);
    NF_LoadExBgPal("bg/blue", BLUE);

    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "up", 256, 256, 0);
    NF_LoadTextFont("fnt/default", "down", 256, 256, 0);

    // Transfer all palettes for background 3
    for (int n = 1; n < 4; n++)
    {
        NF_VramExBgPal(0, 3, n, n);
        NF_VramExBgPal(1, 3, n, n);
    }

    // Create top screen background
    NF_CreateTiledBg(0, 3, "top");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "bottom");

    // Create text layers
    NF_CreateTextLayer(0, 0, 0, "up");
    NF_CreateTextLayer(1, 0, 0, "down");

    // Variables
    int color = RED;
    int x = 0;
    int y = 0;

    while (1)
    {
        // Change the pakette used by this tile
        NF_SetTilePal(0, 3, x, y, color);

        // Print debug information
        char text[50];
        sprintf(text, "X:%02d  Y:%02d   ExPal:%02d  ", x, y, color);
        NF_WriteText(0, 0, 1, 1, text);

        // Calculate the next tile
        x++;
        if (x > 31)
        {
            x = 0;
            y++;
            if (y > 23)
            {
                y = 0;

                // When the bottom of the screen is reached, change to the next
                // extended palette.

                NF_SetExBgPal(1, 3, color);

                color++;
                if (color > BLUE)
                    color = RED;
            }
        }

        // Update copy of the map in VRAM from the copy in RAM
        NF_UpdateVramMap(0, 3);

        // Update text layers
        NF_UpdateTextLayers();

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
