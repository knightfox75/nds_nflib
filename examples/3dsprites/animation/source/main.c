// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// 3D animated sprite example
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define SPRITEMAXNUM 255

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

    // Initialize 2D engine in the bottom
    NF_Set2D(1, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Initialize 3D sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_Init3dSpriteSys();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);
    NF_LoadTiledBg("bg/bg3", "bg3", 256, 256);

    // Load sprite files from NitroFS
    for (int n = 0; n < SPRITEMAXNUM; n++)
        NF_LoadSpriteGfx("sprite/numbers", n, 16, 16);

    NF_LoadSpritePal("sprite/numbers", 0);

    // Transfer the required sprites to VRAM
    for (int n = 0; n < SPRITEMAXNUM; n++)
        NF_Vram3dSpriteGfx(n, n, true);

    NF_Vram3dSpritePal(0, 0);

    // Create backgrounds in both screens
    NF_CreateTiledBg(0, 3, "bg3");
    NF_CreateTiledBg(1, 3, "nfl");

    // Variables
    s16 x[SPRITEMAXNUM];
    s16 y[SPRITEMAXNUM];
    s8 ix[SPRITEMAXNUM];
    s8 iy[SPRITEMAXNUM];
    u16 timer[SPRITEMAXNUM];
    u8 frame[SPRITEMAXNUM];

    // Initialize sprite variables and create the sprites
    for (int n = 0; n < SPRITEMAXNUM; n++)
    {
        x[n] = rand() % 239;
        y[n] = rand() % 175;
        timer[n] = rand() % 20;
        frame[n] = rand() % 10;

        if ((rand() % 100) > 50)
            ix[n] = 1;
        else
            ix[n] = -1;

        if ((rand() % 100) > 50)
            iy[n] = 1;
        else
            iy[n] = -1;

        NF_Create3dSprite(n, n, 0, x[n], y[n]);
        NF_Set3dSpriteFrame(n, frame[n]);
    }

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    while (1)
    {
        // Move sprites
        for (int n = 0; n < SPRITEMAXNUM; n++)
        {
            x[n] += ix[n];
            if ((x[n] < 0) || (x[n] > (255 - NF_3DSPRITE[n].width)))
                ix[n] = -ix[n];

            y[n] += iy[n];
            if ((y[n] < 0) || (y[n] > (191 - NF_3DSPRITE[n].height)))
                iy[n] = -iy[n];

            timer[n]++;
            if (timer[n] > 19)
            {
                timer[n] = 0;
                frame[n]++;
                if (frame[n] > 9)
                    frame[n] = 0;
            }

            NF_Move3dSprite(n, x[n], y[n]);
            NF_Set3dSpriteFrame(n, frame[n]);
        }

        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update textures of any 3D sprite with "keepframes == true"
        NF_Update3dSpritesGfx();
    }

    return 0;
}
