// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of using RAW sounds
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

    // Initialize DS audio hardware
    soundEnable();

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen

    // Initialize top screen text engine
    NF_InitTextSys(0);

    // Initialize audio buffers
    NF_InitRawSoundBuffers();

    // Load background files from NitroFS
    NF_LoadTiledBg("bg/layer3", "moon", 256, 256);

    // Load font files from NitroFS
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);

    // Load audio files from NitroFS
    NF_LoadRawSound("sfx/sample", 0, 11025, 0);
    NF_LoadRawSound("sfx/music", 1, 22050, 0);

    // Create backgrounds
    NF_CreateTiledBg(0, 3, "moon");
    NF_CreateTiledBg(1, 3, "moon");

    // Create text layer
    NF_CreateTextLayer(0, 0, 0, "normal");

    // Write some text in each text layer
    NF_WriteText(0, 0, 1, 1, "L or R play voice.");
    NF_WriteText(0, 0, 1, 2, "A or B STOP/PLAY music.");
    NF_WriteText(0, 0, 1, 4, "L o R reproducir una voz.");
    NF_WriteText(0, 0, 1, 5, "A or B musica STOP/PLAY");

    // Update text layers
    NF_UpdateTextLayers();

    // Start background music
    u8 sound_id = NF_PlayRawSound(1, 127, 64, true, 0);

    while (1)
    {
        scanKeys(); // Read keypad
        u16 newpress = keysDown(); // Keys just pressed

        // Pause music, but don't remove it from memory
        if (newpress & KEY_A)
            soundPause(sound_id);

        // Resume music from the start of the file
        if (newpress & KEY_B)
            soundResume(sound_id);

        // Play a different sound effect on top of the music
        if (newpress & KEY_R)
            NF_PlayRawSound(0, 127, 127, false, 0);
        if (newpress & KEY_L)
            NF_PlayRawSound(0, 127, 0, false, 0);

        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
