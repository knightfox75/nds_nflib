// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
// SPDX-FileContributor: Antonio Niño Díaz, 2025
//
// Affine sprite loading example
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

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

    // Initialize 2D engine in both screens and use mode 0
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    // Initialize sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_InitSpriteSys(0);        // Top screen
    NF_InitSpriteSys(1);        // Bottom screen

    NF_LoadSpriteGfx("sprite/ball", 0, 32, 32);
    NF_LoadSpritePal("sprite/ball", 0);

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(1, 0, 0, true);
    NF_VramSpritePal(1, 0, 0);

    // Setup ball sprites
    NF_CreateSprite(1, 10, 0, 0, 16, 16);
    NF_EnableSpriteRotScale(1, 10, 4, false); // rotation ID 4, no double size

    NF_CreateSprite(1, 20, 0, 0, 128, 16);
    NF_EnableSpriteRotScale(1, 20, 4, true); // rotation ID 4, double size

    NF_CreateSprite(1, 30, 0, 0, 16, 96);
    NF_EnableSpriteRotScale(1, 30, 5, false); // rotation ID 5, no double size

    NF_CreateSprite(1, 40, 0, 0, 128, 96);
    NF_EnableSpriteRotScale(1, 40, 5, true); // rotation ID 5, double size

    int angle = 512;
    int scale = 350;

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        // Rotate and scale sprites

        if (keys & KEY_LEFT)
            angle += 16;
        if (keys & KEY_RIGHT)
            angle -= 16;

        angle &= 511;

        if (keys & KEY_A)
        {
            scale += 2;
            if (scale > 512)
                scale = 512;
        }
        if (keys & KEY_B)
        {
            scale -= 2;
            if (scale < 0)
                scale = 0;
        }

        if (keys & KEY_SELECT)
        {
            NF_SpriteRotScale(1, 4, angle, -scale, -scale);
            NF_SpriteRotScale(1, 5, -angle, -scale, -scale);
        }
        else
        {
            NF_SpriteRotScale(1, 4, angle, scale, scale);
            NF_SpriteRotScale(1, 5, -angle, scale, scale);
        }

        // Update OAM array
        NF_SpriteOamSet(0);
        NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update OAM
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }

    return 0;
}
