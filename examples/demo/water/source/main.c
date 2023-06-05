// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Water reflection effect (with scroll).
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

// Wave effect variables
u16 mapa[32][24];   // Map
s16 bgx[192];       // X scroll of each line
s8 i[192];          // Movement control of each line

// Background control variables
s16 bg_x[4];

// Function that runs after a scanline is drawn. By modifying the values of the
// scroll registers it's possible to add a wave effect.
void hblank_handler(void)
{
    u32 vline = REG_VCOUNT; // Get the current line

    if ((vline >= 138) && (vline <= 159))
    {
        // If the current line is between 138 and 159, apply effect to layer 1
        bgx[vline] += i[vline];

        if ((bgx[vline] < 1) || (bgx[vline] > 63))
            i[vline] *= -1;

        NF_ScrollBg(0, 1, bg_x[1] + ((bgx[vline] / 8) - 4), 0);
    }

    if ((vline >= 160) && (vline <= 191))
    {
        // If the current line is between 160 and 191, apply effect to layer 0
        bgx[vline] += i[vline];

        if ((bgx[vline] < 1) || (bgx[vline] > (vline - 156)))
            i[vline] *= -1;

        NF_ScrollBg(0, 0, bg_x[0] + (((vline - 156) / 2) - bgx[vline]), 0);
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
    NF_LoadTiledBg("bg/layer0", "layer0", 512, 256);
    NF_LoadTiledBg("bg/layer1", "layer1", 512, 256);
    NF_LoadTiledBg("bg/layer2", "layer2", 512, 256);
    NF_LoadTiledBg("bg/layer3", "layer3", 512, 256);
    NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);

    // Create top screen backgrounds
    NF_CreateTiledBg(0, 3, "layer3");
    NF_CreateTiledBg(0, 2, "layer2");
    NF_CreateTiledBg(0, 1, "layer1");
    NF_CreateTiledBg(0, 0, "layer0");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "nfl");

    // Initialize wave effect
    s16 y = 0;
    s8 inc = 1;
    for (y = 138; y < 192; y++)
    {
        // Layer 1 calculations
        if ((y >= 138) && (y <= 159))
        {
            bgx[y] = 32;
            inc *= -1;
            i[y] = inc;
        }

        // Layer 0 calculations
        if ((y >= 160) && (y <= 191))
        {
            bgx[y] = (y - 156) / 2;
            i[y] = inc;
            inc *= -1;
        }
    }

    // Register a function to be called after a screen line has been drawn (when
    // the horizontal blanking period starts)
    irqSet(IRQ_HBLANK, hblank_handler);
    irqEnable(IRQ_HBLANK);

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        // Move background
        if ((keys & KEY_LEFT) && (bg_x[0] > 8))
            bg_x[0]--;
        if ((keys & KEY_RIGHT) && (bg_x[0] < 247))
            bg_x[0]++;

        // Calculate parallax
        bg_x[1] = bg_x[0] / 1.5;
        bg_x[2] = bg_x[1] / 1.5;
        bg_x[3] = bg_x[2] / 1.5;

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Scroll all backgrounds
        NF_ScrollBg(0, 3, bg_x[3], 0);
        NF_ScrollBg(0, 2, bg_x[2], 0);
        NF_ScrollBg(0, 1, bg_x[1], 0);
        NF_ScrollBg(0, 0, bg_x[0], 0);
    }

    return 0;
}
