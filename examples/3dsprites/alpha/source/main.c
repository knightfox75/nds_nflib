// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of using transparency with 3D sprites.
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define MAXSPRITES 8

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
    consoleClear();

    // Initialize 3D engine in the bottom screen in mode 0
    NF_Set3D(1, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen

    // Initialize 3D sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_Init3dSpriteSys();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/nature", "bg3", 256, 256);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/blueball", 0, 64, 64);
    NF_LoadSpritePal("sprite/blueball", 0);

    // Transfer the required sprites to VRAM
    NF_Vram3dSpriteGfx(0, 0, true);
    NF_Vram3dSpritePal(0, 0);

    // Create background
    NF_CreateTiledBg(0, 3, "bg3");

    // Enable alpha blending between 3D sprites over 3D backgrounds
    REG_BLDCNT = BLEND_ALPHA
               | BLEND_SRC_BG0
               | BLEND_DST_BG1 | BLEND_DST_BG2 | BLEND_DST_BG3 | BLEND_DST_BACKDROP;

    // Variables
    s16 x[MAXSPRITES];
    s16 y[MAXSPRITES];
    s16 ix = 4;
    s16 iy = 4;

    // Initialize sprite variables and create the sprites
    for (int n = 0; n < MAXSPRITES; n++)
    {
        x[n] = 128 - 32;
        y[n] = 96 - 32;

        NF_Create3dSprite(n, 0, 0, x[n], y[n]);
    }

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    while (1)
    {
        // Read keys and touchscreen
        scanKeys();
        touchPosition touchscreen;
        touchRead(&touchscreen);
        u16 keys = keysHeld(); // Keys currently pressed

        // Move sprites that follow the main one
        for (int n = MAXSPRITES - 1; n > 0; n--)
        {
            x[n] = x[n - 1];
            y[n] = y[n - 1];

            NF_Blend3dSprite(n, n + 1, 31 - (n * 4));
            NF_Move3dSprite(n, x[n], y[n]);
        }

        // Move the main sprite
        if (keys & KEY_TOUCH)
        {
            x[0] = touchscreen.px - 32;
            y[0] = touchscreen.py - 32;

            if (x[0] < 8)
                x[0] = 8;
            if (x[0] > 183)
                x[0] = 183;

            if (y[0] < 8)
                y[0] = 8;
            if (y[0] > 119)
                y[0] = 119;
        }
        else
        {
            x[0] += ix;
            if ((x[0] < 8) || (x[0] > 183))
                ix = -ix;

            y[0] += iy;
            if ((y[0] < 8) || (y[0] > 119))
                iy = -iy;
        }

        NF_Move3dSprite(0, x[0], y[0]);

        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
