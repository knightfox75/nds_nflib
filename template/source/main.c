// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// NightFox's Lib Template
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>

#include <nf_lib.h>

int main(int argc, char **argv)
{
    consoleDemoInit(); // Initialize console

    printf("\n Hello World!");

    while (1)
    {
        // Wait for the screen refresh
        swiWaitForVBlank();
    }

    // If this is reached, the program will return to the loader if the loader
    // supports it.
    return 0;
}
