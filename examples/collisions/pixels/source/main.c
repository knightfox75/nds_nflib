// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Collision background example
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
    NF_LoadTiledBg("bg/ppc_bg", "bg3", 512, 512);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/pointer", 0, 8, 8);
    NF_LoadSpritePal("sprite/pointer", 0);

    // Load collision map files from NitroFS
    NF_LoadCollisionBg("maps/ppc_cmap", 0, 512, 512);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "bg3");

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(0, 0, 0, true);
    NF_VramSpritePal(0, 0, 0);

    // Create pointer sprite in the bottom screen and set its priority layer
    NF_CreateSprite(0, 0, 0, 0, 124, 92);
    NF_SpriteLayer(0, 0, 3);

    // Movement variables
    s16 x = 128;
    s16 y = 96;
    s16 spr_x = 0;
    s16 spr_y = 0;
    s16 bg_x = 0;
    s16 bg_y = 0;

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        if (keys & KEY_UP)
            y--;
        if (keys & KEY_DOWN)
            y++;
        if (keys & KEY_LEFT)
            x--;
        if (keys & KEY_RIGHT)
            x++;

        // Movement limits
        if (x < 0)
            x = 0;
        if (x > 511)
            x = 511;

        if (y < 0)
            y = 0;
        if (y > 511)
            y = 511;

        // Background position
        bg_x = x - 128;
        if (bg_x < 0)
            bg_x = 0;
        if (bg_x > 256)
            bg_x = 256;

        bg_y = y - 96;
        if (bg_y < 0)
            bg_y = 0;
        if (bg_y > 320)
            bg_y = 320;

        // Sprite position
        spr_x = (x - bg_x) - 4;
        spr_y = (y - bg_y) - 4;
        NF_MoveSprite(0, 0, spr_x, spr_y);

        // Print pointer position
        consoleClear();
        printf("x:%03d  y:%03d \n\n", x, y);

        // Print pixel color
        u8 pixel = NF_GetPoint(0, x, y);
        switch (pixel)
        {
            case 1:
                printf("Tile: Negro / Black");
                break;
            case 2:
                printf("Tile: Rojo / Red");
                break;
            case 3:
                printf("Tile: Verde / Green");
                break;
            case 4:
                printf("Tile: Azul / Blue");
                break;
            default:
                printf("Value: %03d", pixel);
                break;
        }

        // Update OAM array
        NF_SpriteOamSet(0);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update OAM
        oamUpdate(&oamMain);

        // Update scroll
        NF_ScrollBg(0, 3, bg_x, bg_y);
    }

    return 0;
}
