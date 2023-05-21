// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Basic example of text with scroll.
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
    NF_InitTextSys(1);          // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/layer3", "moon", 256, 256);

    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "up", 256, 768, 0);   // Normal text
    NF_LoadTextFont("fnt/default", "down", 768, 256, 2); // Text rotated 180 degrees

    // Create top screen background
    NF_CreateTiledBg(0, 3, "moon");

    // Create bottom screen background
    NF_CreateTiledBg(1, 3, "moon");

    // Create text layers
    NF_CreateTextLayer(0, 0, 0, "up");
    NF_CreateTextLayer(1, 0, 2, "down");

    // Variables
    char mytext[32];
    s16 x = 0;
    s16 y = 0;
    s16 contador = 0;
    s8 delay = 0;

    // Print 96 text lines
    for (int n = 0; n < 96; n++)
    {
        sprintf(mytext," Linea: %02d", n);
        NF_WriteText(0, 0, 0, n, mytext);
        NF_WriteText(1, 0, 0, n, mytext);
    }

    // Update text layers
    NF_UpdateTextLayers();

    while (1)
    {
        delay++;
        if (delay > 10) {

            delay = 0;

            // Print 96 text lines
            for (int n = 0; n < 96; n++)
            {
                sprintf(mytext,"%04d", contador);
                NF_WriteText(0, 0, 26, n, mytext);
                NF_WriteText(1, 0, 18, n, mytext);

                contador++;
                if (contador > 9999)
                    contador = 0;
            }

            // Update text layers
            NF_UpdateTextLayers();

        }

        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        // Scroll top screen text
        if (keys & KEY_UP)
        {
            y -= 2;
            if (y < 0)
                y = 0;
        }
        if (keys & KEY_DOWN)
        {
            y += 2;
            if (y > 575)
                y = 575;
        }

        // Scroll bottom screen text
        if (keys & KEY_LEFT)
        {
            x -= 2;
            if (x < 0)
                x = 0;
        }
        if (keys & KEY_RIGHT)
        {
            x += 2;
            if (x > 511)
                x = 511;
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Scroll text layer
        NF_ScrollBg(0, 0, 0, y);
        NF_ScrollBg(1, 0, x, 63);
    }

    return 0;
}
