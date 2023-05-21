// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// "Reveal" effect
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

    // Initialize 2D engine in both screens. Use mode 0 in the top screen, and
    // mode 5 in the bottom screen.
    NF_Set2D(0, 0);
    NF_Set2D(1, 5);

    // Initialize tiled backgrounds system in the top screen
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);

    // Initialize bitmap backgrounds system in the bottom screen
    NF_Init8bitsBgBuffers();        // Initialize storage buffers
    NF_InitBitmapBgSys(1, 0);       // Initialize 8-bit bitmap backgrounds
    NF_Init8bitsBackBuffer(1);      // Initialize backbuffer
    NF_Enable8bitsBackBuffer(1);    // Enable backbuffer

    // Initialize sprite system in the bottom screen
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_InitSpriteSys(1);

    // Load tiled background files from NitroFS
    NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);

    // Load bitmap background files from NitroFS
    NF_Load8bitsBg("bg/img8b_1", 0);
    NF_Load8bitsBg("bg/img8b_2", 1);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("spr/ball", 0, 32, 32);
    NF_LoadSpritePal("spr/ball", 0);

    // Create bottom screen background
    NF_CreateTiledBg(0, 3, "nfl");

    // Transfer image to the backbuffer of the top screen
    NF_Copy8bitsBuffer(1, 2, 1);
    // Transfer image to layer 3 of the top screen
    NF_Copy8bitsBuffer(1, 1, 0);
    // Copy backbuffer of the top screen to layer 2
    NF_Flip8bitsBackBuffer(1, 0);

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(1, 0, 0, true);
    NF_VramSpritePal(1, 0, 0);

    // Set random seed based on the current time
    srand(time(NULL));

    // State of the balls
    s16 ball_x[32];
    s16 ball_y[32];
    s8 ball_spx[32];
    s8 ball_spy[32];

    for (int n = 0; n < 32; n++)
    {
        ball_x[n] = rand() % 223;
        ball_y[n] = rand() % 159;
        ball_spx[n] = (rand() % 3) + 1;
        ball_spy[n] = (rand() % 3) + 1;
        NF_CreateSprite(1, n, 0, 0, ball_x[n], ball_y[n]);
        NF_SpriteLayer(1, n, 3);
    }

    while (1)
    {
        // Read keys and touchscreen
        scanKeys();
        touchPosition touchscreen;
        touchRead(&touchscreen);
        u16 keys_held = keysHeld();
        u16 keys_down = keysDown();

        // Check if the touch screen is pressed
        if (keys_held & KEY_TOUCH)
        {
            int x = touchscreen.px - 8;
            if (x < 0)
                x = 0;
            if (x > 239)
                x = 239;

            int y = touchscreen.py - 8;
            if (y < 0)
                y = 0;
            if (y > 175)
                y = 175;

            // Draw a 16x16 block around the touchscreen coordinates in the
            // bottom screen.
            for (int dy = 0; dy < 16; dy++)
            {
                for (int dx = 0; dx < 16; dx++)
                {
                    u16 idx = ((y + dy) << 8) + (x + dx);
                    NF_8BITS_BACKBUFFER[1].data[idx] = 0;
                }
            }
        }

        // Press X to reload the top screen image
        if (keys_down & KEY_B)
            NF_Copy8bitsBuffer(1, 2, 1);

        // Copy bottom screen backbuffer to layer 2
        NF_Flip8bitsBackBuffer(1, 0);

        // Move the sprites
        for (int n = 0; n < 32; n++)
        {
            ball_x[n] += ball_spx[n];
            if ((ball_x[n] < 0) || (ball_x[n] > 223))
                ball_spx[n] *= -1;

            ball_y[n] += ball_spy[n];
            if ((ball_y[n] < 0) || (ball_y[n] > 159))
                ball_spy[n] *= -1;

            NF_MoveSprite(1, n, ball_x[n], ball_y[n]);
        }

        // Update OAM array
        NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update OAM
        oamUpdate(&oamSub);
    }

    return 0;
}
