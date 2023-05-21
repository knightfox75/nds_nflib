// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Collision map example.
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

    // Initialize 2D engine in both screens and use mode 0
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Initialize sprite system
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0);        // Top screen
    NF_InitSpriteSys(1);        // Bottom screen

    // Initialize text system
    NF_InitTextSys(0);          // Top screen

    // Initialize collision map buffers
    NF_InitCmapBuffers();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/layer3", "moon", 256, 256);
    NF_LoadTiledBg("bg/colmap", "boxes", 768, 512);

    // Load sprite files from NitroFS
    NF_LoadSpriteGfx("sprite/pointer", 0, 8, 8);
    NF_LoadSpritePal("sprite/pointer", 0);

    // Load text font files from NitroFS
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);

    // Load collision map files from NitroFS
    NF_LoadCollisionMap("maps/cmap", 0, 768, 512);

    // Create top screen background
    NF_CreateTiledBg(0, 3, "moon");

    // Create bottom screen backgrounds
    NF_CreateTiledBg(1, 3, "moon");
    NF_CreateTiledBg(1, 2, "boxes");

    // Create a text layer
    NF_CreateTextLayer(0, 2, 0, "normal");

    // Transfer the required sprites to VRAM
    NF_VramSpriteGfx(1, 0, 0, true);
    NF_VramSpritePal(1, 0, 0);

    // Create pointer sprite in the bottom screen and set its priority layer
    NF_CreateSprite(1, 0, 0, 0, 124, 92);
    NF_SpriteLayer(1, 0, 2);

    // Movement variables
    s16 x = 128;
    s16 y = 96;
    s16 spr_x = 0;
    s16 spr_y = 0;
    s16 bg_x = 0;
    s16 bg_y = 0;

    // Buffer de texto
    char mytext[32];

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

        // Limites del movimiento
        if (x < 0)
            x = 0;
        if (x > 767)
            x = 767;

        if (y < 0)
            y = 0;
        if (y > 511)
            y = 511;

        // Background position
        bg_x = x - 128;
        if (bg_x < 0)
            bg_x = 0;
        if (bg_x > 512)
            bg_x = 512;

        bg_y = y - 96;
        if (bg_y < 0)
            bg_y = 0;
        if (bg_y > 320)
            bg_y = 320;

        // Sprite position
        spr_x = (x - bg_x) - 4;
        spr_y = (y - bg_y) - 4;
        NF_MoveSprite(1, 0, spr_x, spr_y);

        // Print pointer position
        sprintf(mytext,"x:%d  y:%d ", x, y);
        NF_WriteText(0, 2, 1, 1, mytext);

        // Print tile number
        switch (NF_GetTile(0, x, y))
        {
            case 0:
                sprintf(mytext,"Tile: Vacio / Void  ");
                break;
            case 1:
                sprintf(mytext,"Tile: Rojo / Red    ");
                break;
            case 2:
                sprintf(mytext,"Tile: Verde / Green ");
                break;
            case 3:
                sprintf(mytext,"Tile: Azul / Blue   ");
                break;
        }
        NF_WriteText(0, 2, 1, 3, mytext);

        // Update text layers
        NF_UpdateTextLayers();

        // Update OAM array
        NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Update OAM
        oamUpdate(&oamSub);

        // Update background scroll
        NF_ScrollBg(1, 2, bg_x, bg_y);
    }

    return 0;
}
