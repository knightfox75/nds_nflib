// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example of loading 16-bit bitmap backgrounds.
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

    // Initialize 2D engine in both screens and use mode 5
    NF_Set2D(0, 5);
    NF_Set2D(1, 5);

    // Initialize bitmap backgrounds system
    NF_InitBitmapBgSys(0, 1);
    NF_InitBitmapBgSys(1, 1);

    // Initialize storage buffers
    NF_Init16bitsBgBuffers();

    // Load bitmap background files from NitroFS
    NF_Load16bitsBg("bmp/bitmap16", 0);

    // Tranfer image to VRAM of both screens
    NF_Copy16bitsBuffer(0, 0, 0);
    NF_Copy16bitsBuffer(1, 0, 0);

    // It's not needed to use it any longer, so remove it from RAM
    NF_Unload16bitsBg(0);

    while (1)
    {
        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}
