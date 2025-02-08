// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Aarón Caballero Ros, 2025
//
// Loading sprites of all available sizes on both screens and moving it through a grid
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

int main(int argc, char **argv){
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
    NF_InitSpriteBuffers();     // Initialize storage buffers
    NF_InitSpriteSys(0);        // Top screen
    NF_InitSpriteSys(1);        // Bottom screen

    // Load background file from NitroFS
    NF_LoadTiledBg("bg/white_bg", "white_bg", 256, 256);

    //Loading the color palette for the sprites (In this case all sprites have the same palette)
    NF_LoadSpritePal("sprite/64x64",0);
    NF_VramSpritePal(0, 0, 0);
    NF_VramSpritePal(1, 0, 0);

    //Making a array of available sizes for the sprites
    struct{
        u8 x,y;
    }
    available_sizes[] ={
        {8, 8},
        {16, 16},
        {32, 32},
        {64, 64},
        {16, 8},
        {32, 8},
        {32, 16},
        {64, 32},
        {8, 16},
        {8, 32},
        {16, 32},
        {32, 64}
    };
    u8 n_available_sizes = sizeof(available_sizes) / sizeof(available_sizes[0]);

    // Iterate over all available sizes
    for (int i = 0; i < n_available_sizes; i++) {
        //Getting name of the sprite file , all are in the format "sprite/size_x x size_y". (ej:"sprite/64x64")
        char filename[64];
        sprintf(filename, "sprite/%dx%d", available_sizes[i].x, available_sizes[i].y); 

        // Load sprite files from NitroFS
        NF_LoadSpriteGfx(filename, i, available_sizes[i].x, available_sizes[i].y);
        
        // Transfer the sprite to VRAM in both screans
        NF_VramSpriteGfx(0, i, i, false);
        NF_VramSpriteGfx(1, i, i, false);
        
        // Cordinates to place the sprites in a grid 4x3, with 64x64 cell for each sprite
        u8 p_x = 64*(i%4); u8 p_y = 64*(i/4); 

        //Creating the sprite in both screans
        NF_CreateSprite( 0, i, i, 0, p_x, p_y ); 
        NF_CreateSprite( 1, i, i, 0, p_x, p_y );
    }
    NF_CreateTiledBg(0, 3, "white_bg");
    NF_CreateTiledBg(1, 3, "white_bg");

    
    u8 current_vblank=0; //Counter of vblanks
    u8 current_displacement=-1; //Displacement of the sprite respect to the grid
    while (1){

        //Moving the sprite every second
        if(current_vblank==0){
            if(current_displacement==-1) current_displacement=0; //In this way avoid displacement at the beginning
            else{
                current_displacement++;
                for (u8 i = 0; i < n_available_sizes; i++) {
                    
                    //Moving the sprite in the top screen
                    u8 dp_i = (i+current_displacement)%n_available_sizes; //Getting index displaced
                    u8 p_x = 64*(dp_i%4); u8 p_y = 64*(dp_i/4); //Getting the position of the sprite in the grid with displacement forward
                    
                    //Moving the sprite in both screens
                    NF_MoveSprite(0, i, p_x, p_y);
                    
                    //Doing the same for the bottom screen but with a backwards displacement
                    dp_i = ( (n_available_sizes+i) -current_displacement )%n_available_sizes; //Adding n_available_sizes to avoid negative values
                    p_x = 64*(dp_i%4); p_y = 64*(dp_i/4);
                    NF_MoveSprite(1, i, p_x, p_y);
                }
                current_displacement%=n_available_sizes;
            }
        }

        // Update OAM array
        NF_SpriteOamSet(0);
        NF_SpriteOamSet(1);

        // Wait for the screen refresh
        swiWaitForVBlank();
        current_vblank++;
        current_vblank%=60; // 60 vblanks = 1 second

        // Update OAM
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }

    return 0;
}
