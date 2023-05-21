// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Basic background palette effects.
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

    // Initialize text system
    NF_InitTextSys(0);

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/layer3", "moon", 256, 256);

    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);
    NF_LoadTextFont("fnt/default", "right", 256, 256, 1);
    NF_LoadTextFont("fnt/default", "left", 256, 256, 2);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "moon");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "moon");

    // Create text layers
    NF_CreateTextLayer(0, 0, 0, "normal");
    NF_CreateTextLayer(0, 1, 1, "right");
    NF_CreateTextLayer(0, 2, 2, "left");

    // Scroll text background to offset the unused space in the background
    NF_ScrollBg(0, 2, 0, 63);

    // Print text in all layers
    NF_BgSetPalColor(0, 0, 1, 31, 31, 0); // Set color
    NF_WriteText(0, 0, 1, 1, "Hola Mundo!\n Hello World!"); // Normal orientation
    NF_BgEditPalColor(0, 1, 1, 0, 31, 31); // Set color
    NF_BgUpdatePalette(0, 1);
    NF_WriteText(0, 1, 1, 1, "Hola Mundo!\n Hello World!"); // Rotated clockwise
    NF_WriteText(0, 2, 1, 1, "Hola Mundo!\n Hello World!"); // Rotated counter-clockwise

    // Update all text layers
    NF_UpdateTextLayers();

    // Variables
    u32 myvar = 0;
    u8 speed = 0;

    while (1)
    {
        myvar++;

        char mytext[32];
        snprintf(mytext, sizeof(mytext), "Counter: %lu", myvar);
        NF_WriteText(0, 0, 1, 5, mytext);
        NF_WriteText(0, 1, 1, 5, mytext);
        NF_WriteText(0, 2, 1, 5, mytext);

        // Update text layers
        NF_UpdateTextLayers();

        // Sunset effect by modifying the palette
        speed++;
        if (speed > 30)
        {
            speed = 0;
            for (int n = 1; n < 256; n++)
            {
                u8 r, g, b;
                NF_BgGetPalColor(1, 3, n, &r, &g, &b);
                int cR = r;
                int cG = g;
                int cB = b;

                cR--;
                if (cR < 0)
                    cR = 0;

                cG--;
                if (cG < 0)
                    cG = 0;

                NF_BgEditPalColor(1, 3, n, cR, cG, cB);
            }
            NF_BgUpdatePalette(1, 3);
        }

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
