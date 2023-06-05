// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
//
// 3D sprite deletion example.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

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

    // Transfer the required sprite to VRAM
    NF_Vram3dSpriteGfx(0, 0, true);
    NF_Vram3dSpritePal(0, 0);

    // Create background in the top screen
    NF_CreateTiledBg(0, 3, "bg3");

    // Create a few sprites
    NF_Create3dSprite(0, 0, 0, 0, 0);
    NF_Create3dSprite(1, 0, 0, 80, 0);
    NF_Create3dSprite(2, 0, 0, 160, 0);

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    consoleClear();
    printf("A: Create and delete sprites\n\n\n");

    while (1)
    {
        // Read keys
        scanKeys();
        u16 keys = keysDown();

        if (keys & KEY_A)
            break;

        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    // Delete one sprite
    NF_Delete3dSprite(1);

    // Overwrite a sprite that exists
    NF_Create3dSprite(2, 0, 0, 160, 80);

    // Create a new sprite
    NF_Create3dSprite(3, 0, 0, 0, 80);

    // Sort their priorites based on their IDs (lower IDs have higher priority)
    NF_Sort3dSprites();

    printf("Done!");

    while (1)
    {
        // Draw all 3D sprites
        NF_Draw3dSprites();

        // Tell the GPU to draw the scene and wait until it's done
        glFlush(0);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
