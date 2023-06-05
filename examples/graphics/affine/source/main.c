// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Affine backgrounds example
// http://www.nightfoxandco.com/

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

// Function that runs after a scanline is drawn. By modifying the values of the
// fade registers it's possible to add a shading effect.
void hblank_handler(void)
{
    u32 vline = REG_VCOUNT; // Get the current line

    // If the current line is inside the screen
    if (vline < 192)
    {
        // Calculate fade value based on the line
        int fade = 0x0F - (((vline + 1) * 16) / 192);

        REG_BLDY = fade / 2;   // Top screen
        REG_BLDY_SUB = fade;    // Bottom screen
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

    // Initialize 2D engine in both screens and use mode 2
    NF_Set2D(0, 2);
    NF_Set2D(1, 2);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitAffineBgSys(0);      // Top screen
    NF_InitAffineBgSys(1);      // Bottom screen

    // Load background files from NitroFS
    NF_LoadAffineBg("bg/waves512", "waves", 512, 512);
    NF_LoadAffineBg("bg/navygrid", "grid", 256, 256);
    NF_LoadAffineBg("bg/flag512", "flag", 512, 512);

    // Create top screen backgrounds
    NF_CreateAffineBg(0, 3, "waves", 1);
    NF_CreateAffineBg(0, 2, "grid", 0);

    // Create bottom screen backgrounds
    NF_CreateAffineBg(1, 3, "flag", 1);

    // Register a function to be called after a screen line has been drawn (when
    // the horizontal blanking period starts)
    irqSet(IRQ_HBLANK, hblank_handler);
    irqEnable(IRQ_HBLANK);

    // Movement variables
    s32 x = 0;
    s32 y = 0;
    s32 angle = 0;
    s32 zoom = 256;

    // Setup fading registers. The top screen fades to white and the bottom one
    // fades to black.
    REG_BLDCNT = BLEND_FADE_WHITE | BLEND_SRC_BG3;
    REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3;

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        // Move center of the backgrounds
        if (keys & KEY_UP)
        {
            y -= 2;
            if (y < 0)
                y = 0;
        }
        if (keys & KEY_DOWN)
        {
            y += 2;
            if (y > 512)
                y = 512;
        }
        if (keys & KEY_LEFT)
        {
            x -= 2;
            if (x < 0)
                x = 0;
        }
        if (keys & KEY_RIGHT)
        {
            x += 2;
            if (x > 512)
                x = 512;
        }

        // Change zoom
        if (keys & KEY_A)
        {
            zoom -= 2;
            if (zoom < 0)
                zoom = 0;
        }
        if (keys & KEY_B)
        {
            zoom += 2;
            if (zoom > 512)
                zoom = 512;
        }

        // Change angle
        if (keys & KEY_X)
        {
            angle -= 2;
            if (angle < 0)
                angle += 2048;
        }
        if (keys & KEY_Y)
        {
            angle += 2;
            if (angle > 2048)
                angle -= 2048;
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update parameters of the affine backgrounds
        for (int n = 0; n < 2; n++)
        {
            NF_AffineBgTransform(n, 3, zoom, zoom, 0, 0);
            NF_AffineBgCenter(n, 3, x, y);
            NF_AffineBgMove(n, 3, 0, 0, angle);
        }
    }

    return 0;
}
