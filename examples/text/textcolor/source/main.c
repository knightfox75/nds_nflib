// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of text with extended palettes
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

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
    NF_InitTextSys(1);          // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);

    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "down", 256, 256, 0);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "nfl");

    // Create a text layer
    NF_CreateTextLayer(1, 0, 0, "down");

    // Define a few colors for the text font (bottom screen, layer 3)
    NF_DefineTextColor(1, 0, 1, 31, 0, 0); // Red
    NF_DefineTextColor(1, 0, 2, 0, 31, 0); // Green
    NF_DefineTextColor(1, 0, 3, 0, 0, 31); // Blue

    // Generate a few colors randomly
    srand(time(NULL));
    for (int n = 4; n < 16; n++)
    {
        NF_DefineTextColor(1, 0, n,
                           (rand() % 31) + 1,
                           (rand() % 31) + 1,
                           (rand() % 31) + 1);
    }

    // Print text with predefined colors
    NF_WriteText(1, 0, 1, 1, "Defecto / Default"); // Print text with predefined colors
    NF_SetTextColor(1, 0, 1);                   // Set color for new text
    NF_WriteText(1, 0, 1, 2, "Rojo / Red");     // Print text with the new color
    NF_SetTextColor(1, 0, 2);
    NF_WriteText(1, 0, 1, 3, "Verde / Green");
    NF_SetTextColor(1, 0, 3);
    NF_WriteText(1, 0, 1, 4, "Azul / Blue");

    // Print text with random colors
    for (int n = 4; n < 16; n++)
    {
        NF_SetTextColor(1, 0, n);
        NF_WriteText(1, 0, 1, n + 1, "Random color");
    }

    // Restore default text color
    NF_SetTextColor(1, 0, 0);

    // Update text layers
    NF_UpdateTextLayers();

    // Variables
    int speed = 0;

    while (1)
    {
        speed++;
        if (speed > 60)
        {
            speed = 0;

            // Generate new colors randomly
            for (int n = 4; n < 16; n++)
            {
                NF_DefineTextColor(1, 0, n,
                                   (rand() % 31) + 1,
                                   (rand() % 31) + 1,
                                   (rand() % 31) + 1);
            }

            // Print text with the new random colors
            for (int n = 4; n < 16; n++)
            {
                NF_SetTextColor(1, 0, n);
                NF_WriteText(1, 0, 1, n + 1, "Random color");
            }

            // Update text layers
            NF_UpdateTextLayers();
        }

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
