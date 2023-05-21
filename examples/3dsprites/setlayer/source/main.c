// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of setting the layer of 3D sprites.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define SPRITEMAXNUM 32

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

    // Initialize 2D engine in the bottom screen in mode 0
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
    NF_LoadTiledBg("bg/bg2", "bg2", 1536, 256);
    NF_LoadTiledBg("bg/bg1", "bg1", 2048, 256);

    // Load sprite files from NitroFS
    for (int n = 0; n < SPRITEMAXNUM; n++)
        NF_LoadSpriteGfx("sprite/numbers", n, 16, 16);

    NF_LoadSpritePal("sprite/numbers", 0);

    // Transfer the required sprites to VRAM
    for (int n = 0; n < SPRITEMAXNUM; n++)
        NF_Vram3dSpriteGfx(n, n, true);

    NF_Vram3dSpritePal(0, 0);

    // Create backgrounds of both screens
    NF_CreateTiledBg(0, 3, "bg3");
    NF_CreateTiledBg(0, 2, "bg2");
    NF_CreateTiledBg(0, 1, "bg1");
    NF_CreateTiledBg(1, 3, "nfl");

    // Variables
    int x[SPRITEMAXNUM];
    int y[SPRITEMAXNUM];
    int ix[SPRITEMAXNUM];
    int iy[SPRITEMAXNUM];
    u32 timer[SPRITEMAXNUM];
    u32 frame[SPRITEMAXNUM];

    int bg_x = 128;
    int bg_speed = 1;
    int layer = 0;

    // Inicializa las variables
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

        // Create 3D sprites
        NF_Create3dSprite(n, n, 0, x[n], y[n]);
        NF_Set3dSpriteFrame(n, frame[n]);
    }

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        // Move all sprites
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

        // Move background
        bg_x += bg_speed;
        if ((bg_x < 1) || (bg_x > 1662))
            bg_speed = -bg_speed;

        // Change the layer where the 3D sprites are being drawn
        if (keys & KEY_A)
        {
            layer++;
            if (layer > 2)
                layer = 2;
            NF_3dSpritesLayer(layer);
        }
        if (keys & KEY_B)
        {
            layer--;
            if (layer < 0)
                layer = 0;
            NF_3dSpritesLayer(layer);
        }

        // Draw 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update background scroll during vertical blanking to avoid tearing
        NF_ScrollBg(0, 1, bg_x, 0);
        NF_ScrollBg(0, 2, bg_x / 1.5, 0);

        // Update textures of any 3D sprite with "keepframes == true"
        NF_Update3dSpritesGfx();
    }

    return 0;
}
