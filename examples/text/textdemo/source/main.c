// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Basic text example.
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
    NF_InitTextSys(0);          // Top screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/layer3", "moon", 256, 256);

    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0); // Load normal text
    NF_LoadTextFont("fnt/default", "right", 256, 256, 1);  // Load rotated text
    NF_LoadTextFont("fnt/default", "left", 256, 256, 2);   // Load rotated text

    // Create top screen background
    NF_CreateTiledBg(0, 3, "moon");

    // Create bottom screen background
    NF_CreateTiledBg(1, 3, "moon");

    // Create text layers
    NF_CreateTextLayer(0, 0, 0, "normal");
    NF_CreateTextLayer(0, 1, 1, "right");
    NF_CreateTextLayer(0, 2, 2, "left");

    // Scroll text background to offset the unused space in the background
    // that uses text rotated to the left.
    NF_ScrollBg(0, 2, 0, 63);

    // Print text in all layers
    NF_WriteText(0, 0, 1, 1, "Hola Mundo!\n Hello World!");
    NF_WriteText(0, 1, 1, 1, "Hola Mundo!\n Hello World!");
    NF_WriteText(0, 2, 1, 1, "Hola Mundo!\n Hello World!");

    // Update text layers
    NF_UpdateTextLayers();

    // Variables
    u32 myvar = 0;

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

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
