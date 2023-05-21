// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of per-pixel collisions.
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

    // Initialize 2D engine in the top screen and use mode 0
    NF_Set2D(0, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen

    // Initialize sprite system
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0);        // Top screen

    // Initialize collision map buffers
    NF_InitCmapBuffers();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/pdemo_bg", "bg3", 256, 256);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/whiteball", 0, 16, 16);
    NF_LoadSpritePal("sprite/whiteball", 0);

    // Load collision map files from NitroFS
    NF_LoadCollisionBg("maps/pdemo_colmap", 0, 256, 256);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "bg3");

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(0, 0, 0, true);
    NF_VramSpritePal(0, 0, 0);

    // Create sprites and set their priority layer
    for (int b = 0; b < 3; b++)
    {
        NF_CreateSprite(0, b, 0, 0, -16, -16);
        NF_SpriteLayer(0, b, 3);
    }

    // Location of the sprites
    s16 x[3] = { 32, 228, 10 };
    s16 y[3] = { -16, 32, 100 };

    // Coordinates of the sprite that have to be used as collision points
    s16 py[16] = {
        11, 13, 14, 15, 15, 16, 16, 16, 16, 16, 16, 15, 15, 14, 13, 11
    };

    while (1)
    {
        // Clear text console
        consoleClear();

        bool down = false;
        bool left = false;
        bool right = false;

        // Handle collisions and fall of all balls
        for (int b = 0; b < 3; b++)
        {
            down = true; // If this remains true, there is no collision

            // Check all pixels of the lower half of the ball to see if there is
            // a collision.
            for (int n = 0; n < 16; n++)
            {
                // Blue pixels are index 4
                if (NF_GetPoint(0, (x[b] + n), (y[b] + py[n])) == 4)
                    down = false;
            }

            // Check for collisions left and right of the ball
            right = true;
            left = true;

            // Falling to the left
            if (NF_GetPoint(0, (x[b] - 1), (y[b] + 16)) == 4)
                left = false;

            // Falling to the right
            if (NF_GetPoint(0, (x[b] + 16), (y[b] + 16)) == 4)
                right = false;

            // On free fall, don't move on the X axis
            if (left && right)
            {
                right = false;
                left = false;
            }

            // Free fall
            if (down)
                y[b]++;

            // Move right
            if (right)
                x[b]++;

            // Move left
            if (left)
                x[b]--;

            // If the ball exits the screen from the bottom move it to the top
            if (y[b] > 192)
            {
                x[b] = 32;
                y[b] = -16;
            }

            // Set the sprite position
            NF_MoveSprite(0, b, x[b], y[b]);

            // Print sprite coordinates
            printf("x:%03d  y:%03d\n", x[b], y[b]);

        }

        // Update OAM array
        NF_SpriteOamSet(0);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update OAM
        oamUpdate(&oamMain);
    }

    return 0;
}
