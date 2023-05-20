// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Priority swap example.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define MAXSPRITES 32

int main(int argc, char **argv)
{
    // Set random seed based on the current time
    srand(time(NULL));

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

    // Initialize 3D engine in the top screen in mode 0
    NF_Set3D(0, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen

    // Initialize 3D sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_Init3dSpriteSys();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/bg3", "bg3", 256, 256);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/blueball", 0, 64, 64);
    NF_LoadSpritePal("sprite/blueball", 0);
    NF_LoadSpriteGfx("sprite/redcar", 1, 128, 64);
    NF_LoadSpritePal("sprite/redcar", 1);

    // Transfer the required sprites to VRAM
    NF_Vram3dSpriteGfx(0, 0, true);
    NF_Vram3dSpritePal(0, 0);
    NF_Vram3dSpriteGfx(1, 1, true);
    NF_Vram3dSpritePal(1, 1);

    // Create background
    NF_CreateTiledBg(0, 3, "bg3");

    // Variables
    s16 x[MAXSPRITES];
    s16 y[MAXSPRITES];
    s16 ix[MAXSPRITES];
    s16 iy[MAXSPRITES];

    // Initialize sprite variables and create the sprites
    for (int n = 0; n < MAXSPRITES; n++)
    {
        int r;

        if ((n == 0) || (n == (MAXSPRITES - 1)))
            r = 1;
        else
            r = 0;

        x[n] = (rand() % 128) + 1;
        y[n] = (rand() % 112) + 1;

        if ((rand() % 100) > 50)
            ix[n] = 1;
        else
            ix[n] = -1;

        if ((rand() % 100) > 50)
            iy[n] = 1;
        else
            iy[n] = -1;

        NF_Create3dSprite(n, r, r, x[n], y[n]);
    }

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    while (1)
    {
        // Read keys
        scanKeys();
        u16 keys = keysDown();

        // Move sprites
        for (int n = 0; n < MAXSPRITES; n++)
        {
            x[n] += ix[n];
            if ((x[n] < 1) || (x[n] > (255 - NF_3DSPRITE[n].width)))
                ix[n] = -ix[n];

            y[n] += iy[n];
            if ((y[n] < 1) || (y[n] > (191 - NF_3DSPRITE[n].height)))
                iy[n] = -iy[n];

            NF_Move3dSprite(n, x[n], y[n]);
        }

        // Show or hide sprites with even ID
        if (keys & KEY_A)
            NF_Swap3dSpritePriority(0, MAXSPRITES - 1);

        if (keys & KEY_B)
            NF_Swap3dSpritePriority(MAXSPRITES - 1, 0);

        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Print debug information
        consoleClear();
        printf("\nSprite Id%02d is on %02d priority\n", 0, NF_3DSPRITE[0].prio);
        printf("Sprite Id%02d is on %02d priority\n\n",
               MAXSPRITES - 1, NF_3DSPRITE[MAXSPRITES - 1].prio);
        printf("A or B - Swap 0 & %d", MAXSPRITES - 1);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
