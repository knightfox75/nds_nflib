// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of animating tiled backgrounds.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define SPEED 2         // Scroll speed
#define MAP_X 16        // Map width (animated blocks are 4x4, 64 / 4 = 16)
#define MAP_Y 16        // Map height (animated blocks are 4x4, 64 / 4 = 16)
#define WATER_FRAMES 50 // Number of frames of the animation
#define WATER_DELAY 10  // Animation speed (number of frames to wait to change)

// Global variables
u8 WATER_SPEED;
u8 WATER_FRAME[MAP_X][MAP_Y]; // 512 pixels, 64 tiles, every frame is 4x4 tiles

// Initialize animated background
void InitAnimatedBg(void)
{
    // Initialize water animations
    for (int y = 0; y < MAP_Y; y++)
    {
        for (int x = 0; x < MAP_X; x++)
            WATER_FRAME[x][y] = rand() % (WATER_FRAMES + 1);
    }

    // Generate map of water background
    for (int y = 0; y < MAP_Y; y++)
    {
        for (int x = 0; x < MAP_X; x++)
        {
            // Calculate the map 4x4 region to fill
            int start_x = x * 4;
            int end_x = start_x + 4;
            int start_y = y * 4;
            int end_y = start_y + 4;

            // Displacement
            int n = 0;

            // Fill the 4x4 region
            for (int tile_y = start_y; tile_y < end_y; tile_y++)
            {
                for (int tile_x = start_x; tile_x < end_x; tile_x++)
                {
                    // The animation has 11 frames. Any number over the limit
                    // will display the "rest" frame.
                    int frame = WATER_FRAME[x][y];
                    if (frame > 10)
                        frame = 10;

                    NF_SetTileOfMap(1, 3, tile_x, tile_y, (frame * 16) + n);
                    n++;
                }
            }
        }
    }

    // Update map in VRAM
    NF_UpdateVramMap(1, 3);
}

// Handle tiled background animation
void AnimateWater(void)
{
    // Check if we need to do an animation step
    WATER_SPEED++;

    if (WATER_SPEED < WATER_DELAY)
        return;

    WATER_SPEED = 0;

    // Change the required tiles for the animation
    for (int y = 0; y < MAP_Y; y++)
    {
        for (int x = 0; x < MAP_X; x++)
        {
            // Calculate the map 4x4 region to fill
            int start_x = x * 4;
            int end_x = start_x + 4;
            int start_y = y * 4;
            int end_y = start_y + 4;

            // CAlculate the frame to draw
            WATER_FRAME[x][y]++;

            if (WATER_FRAME[x][y] > WATER_FRAMES)
                WATER_FRAME[x][y] = 0;

            if (WATER_FRAME[x][y] <= 10)
            {
                // Displacement
                int n = 0;

                // Fill the 4x4 region
                for (int tile_y = start_y; tile_y < end_y; tile_y++)
                {
                    for (int tile_x = start_x; tile_x < end_x; tile_x++)
                    {
                        NF_SetTileOfMap(1, 3, tile_x, tile_y, (WATER_FRAME[x][y] * 16) + n);
                        n++;
                    }
                }
            }
        }
    }

    // Update map in VRAM
    NF_UpdateVramMap(1, 3);
}

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

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);
    NF_LoadTilesForBg("bg/water", "water", 512, 512, 0, 175);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "nfl");
    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "water");

    // Initialize background animation
    InitAnimatedBg();

    // Variables
    s16 bg_x = 0;
    s16 bg_y = 0;

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld();

        // Scroll background
        if (keys & KEY_LEFT)
        {
            bg_x -= SPEED;
            if (bg_x < 0)
                bg_x = 0;
        }

        if (keys & KEY_RIGHT)
        {
            bg_x += SPEED;
            if (bg_x > 255)
                bg_x = 255;
        }

        if (keys & KEY_UP)
        {
            bg_y -= SPEED;
            if (bg_y < 0)
                bg_y = 0;
        }

        if (keys & KEY_DOWN)
        {
            bg_y += SPEED;
            if (bg_y > 255)
                bg_y = 255;
        }

        // Animate background
        AnimateWater();

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Scroll background
        NF_ScrollBg(1, 3, bg_x, bg_y);
    }

    return 0;
}
