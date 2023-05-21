// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example to load 8, 16 and 24 bit BMP files.
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

#include <nf_lib.h>

#define MAX_BMP_FILES 3

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

    // Initialize backbuffers
    NF_Init16bitsBackBuffer(0);
    NF_Init16bitsBackBuffer(1);

    // Enable backbuffers
    NF_Enable16bitsBackBuffer(0);
    NF_Enable16bitsBackBuffer(1);

    // Load bitmap files from NitroFS
    NF_LoadBMP("bmp/bmp8bits", 0);
    NF_LoadBMP("bmp/bmp16bits", 1);
    NF_LoadBMP("bmp/bmp24bits", 2);

    // Variables
    s8 img1 = 0;
    s8 img2 = 1;

    // Draw screens on the backbuffers
    NF_Draw16bitsImage(0, img1, 0, 0, false);
    NF_Draw16bitsImage(1, img2, 0, 0, false);

    while (1)
    {
        scanKeys(); // Read keypad
        u16 keys = keysHeld(); // Keys currently pressed

        if ((keys & KEY_UP) || (keys & KEY_DOWN))
        {
            if (keys & KEY_UP)
            {
                img1--;
                img2--;
                if (img1 < 0)
                    img1 = (MAX_BMP_FILES - 1);
                if (img2 < 0)
                    img2 = (MAX_BMP_FILES - 1);
            }
            if (keys & KEY_DOWN)
            {
                img1++;
                img2++;
                if (img1 > (MAX_BMP_FILES - 1))
                    img1 = 0;
                if (img2 > (MAX_BMP_FILES - 1))
                    img2 = 0;
            }

            // Draw images on the backbuffers
            NF_Draw16bitsImage(0, img1, 0, 0, false);
            NF_Draw16bitsImage(1, img2, 0, 0, false);
        }

        // Wait for the screen refresh
        swiWaitForVBlank();

        // Send backbuffers to the screen
        NF_Flip16bitsBackBuffer(0);
        NF_Flip16bitsBackBuffer(1);
    }

    return 0;
}
