// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Background loading example
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define SPEED 2 // Scroll speed

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
    NF_LoadTiledBg("bg/bg3", "layer_3", 256, 256);
    NF_LoadTiledBg("bg/bg2", "layer_2", 1024, 256);
    NF_LoadTiledBg("bg/bg1", "layer_1", 1536, 256);
    NF_LoadTiledBg("bg/bg0", "layer_0", 2048, 256);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "nfl");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "layer_3");
    NF_CreateTiledBg(1, 2, "layer_2");
    NF_CreateTiledBg(1, 1, "layer_1");
    NF_CreateTiledBg(1, 0, "layer_0");

    // Variables
    s16 bg_x[4] = { 0, 0, 0, 0 };
    s16 bg_y[4] = { 0, 0, 0, 0 };

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        // If pressing left
        if (keys & KEY_LEFT)
        {
            bg_x[0] -= SPEED;
            if (bg_x[0] < 0)
                bg_x[0] = 0;
        }

        // If pressing right
        if (keys & KEY_RIGHT)
        {
            bg_x[0] += SPEED;
            if (bg_x[0] > 1663)
                bg_x[0] = 1663; // Max scroll of layer 0
        }

        // Scroll other layers less than layer 0
        bg_x[1] = (int)(bg_x[0] / 1.5);
        bg_x[2] = (int)(bg_x[1] / 1.5);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update background scroll during vertical blanking to avoid tearing
        for (int n = 0; n < 4; n++)
            NF_ScrollBg(1, n, bg_x[n], bg_y[n]);
    }

    return 0;
}
