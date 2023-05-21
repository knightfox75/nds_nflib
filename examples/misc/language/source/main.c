// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011
//
// Example that shows how to get the user language
// http://www.nightfoxandco.com

#include <stdio.h>

#include <nds.h>

#include <nf_lib.h>

int main(int argc, char **argv)
{
    // Initialize 2D hardware and default console
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    consoleDemoInit();

    // Print on the screen the user language
    switch (NF_GetLanguage())
    {
        case 0:
            printf("Japanese");
            break;
        case 1:
            printf("English");
            break;
        case 2:
            printf("French");
            break;
        case 3:
            printf("German");
            break;
        case 4:
            printf("Italian");
            break;
        case 5:
            printf("Spanish");
            break;
        case 6:
            printf("Chinese");
            break;
        default:
            printf("Error");
            break;
    }

    while (1)
    {
        swiWaitForVBlank();
    }

    return 0;
}
