// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of mixing bitmap and tiled backgrounds.
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

    // Initialize 2D engine in both screens and use mode 5. Layers 0 to 2 are
    // tiled backgrounds, layer 3 is an 8-bit bitmap background.
    NF_Set2D(0, 5);
    NF_Set2D(1, 5);

    // Initialize mixed background system
    NF_InitMixedBgSys(0);
    NF_InitMixedBgSys(1);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();

    // Initialize text system
    NF_InitTextSys(0);          // Top screen

    // Initialize storage buffers
    NF_Init8bitsBgBuffers();

    // Initialize sprite system
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_InitSpriteSys(0);        // Top screen
    NF_InitSpriteSys(1);        // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/bg0", "tiled_bg", 1024, 256);

    // Load text font files from NitroFS
    NF_LoadTextFont16("fnt/font16", "text_bg0", 256, 256, 0);
    NF_LoadTextFont16("fnt/font16", "text_bg1", 256, 256, 0);

    // Load bitmap files from NitroFS
    NF_Load8bitsBg("bmp/img8b_1", 0);
    NF_Load8bitsBg("bmp/img8b_2", 1);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/ball", 0, 32, 32);
    NF_LoadSpritePal("sprite/ball", 0);

    // Create bottom screen background
    NF_CreateTiledBg(1, 0, "tiled_bg");

    // Create text layers
    NF_CreateTextLayer16(0, 0, 0, "text_bg0");
    NF_CreateTextLayer16(0, 1, 0, "text_bg1");

    // Write some text in the top screen in white
    NF_WriteText16(0, 0, 1, 1, "Text over bitmap");
    NF_WriteText16(0, 0, 1, 3, "Text on layers 0 & 1");
    NF_WriteText16(0, 0, 1, 4, "Bitmap on layer 3");

    // Write the same text in another layer in black, this will be the shadow
    NF_DefineTextColor(0, 1, 1, 0, 0, 0); // Black
    NF_SetTextColor(0, 1, 1);
    NF_WriteText16(0, 1, 1, 1, "Text over bitmap");
    NF_WriteText16(0, 1, 1, 3, "Text on layers 0 & 1");
    NF_WriteText16(0, 1, 1, 4, "Bitmap on layer 3");

    // Small scroll for the black background to simulate the shadow
    NF_ScrollBg(0, 0, 1, 1);

    // Update text layers
    NF_UpdateTextLayers();

    // Transfer image to layer 3 of the top screen
    NF_Copy8bitsBuffer(0, 1, 0);

    // Transfer image to layer 3 of the bottom screen
    NF_Copy8bitsBuffer(1, 1, 1);

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(0, 0, 0, true); // Ball: Keep all frames in VRAM
    NF_VramSpritePal(0, 0, 0);
    NF_VramSpriteGfx(1, 0, 0, true); // Ball: Keep all frames in VRAM
    NF_VramSpritePal(1, 0, 0);

    // Setup and create ball sprites
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
        NF_CreateSprite(0, n, 0, 0, ball_x[n], ball_y[n]);
        NF_CreateSprite(1, n, 0, 0, ball_x[n], ball_y[n]);
        NF_SpriteLayer(0, n, 3);
        NF_SpriteLayer(1, n, 3);
    }

    while (1)
    {
        // Move balls
        for (int n = 0; n < 32; n++)
        {
            ball_x[n] += ball_spx[n];
            if ((ball_x[n] < 0) || (ball_x[n] > 223))
                ball_spx[n] *= -1;

            ball_y[n] += ball_spy[n];
            if ((ball_y[n] < 0) || (ball_y[n] > 159))
                ball_spy[n] *= -1;

            NF_MoveSprite(0, n, ball_x[n], ball_y[n]);
            NF_MoveSprite(1, n, ball_x[n], ball_y[n]);
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
