// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Common 3D functions
// http://www.nightfoxandco.com/

#include <nds.h>

#include "nf_2d.h"
#include "nf_3d.h"
#include "nf_basic.h"

void NF_Set3D(int screen, u32 mode)
{
    // Only the main engine can use 3D, so we need to swap the screens if the
    // user wants the 3D output to be in the screen that isn't the main one.
    if (screen == 0)
        lcdMainOnTop();
    else
        lcdMainOnBottom();

    // Use the selected 3D mode
    switch (mode)
    {
        case 0:
            videoSetMode(MODE_0_3D);
            break;
        case 2:
            videoSetMode(MODE_2_3D);
            break;
        case 5:
            videoSetMode(MODE_5_3D);
            break;
    }
}

void NF_InitOpenGL(void)
{
    // Initialize internal OpenGL state
    glInit();

    // Define viewport to the complete screen
    glViewport(0, 0, 255, 191);

    // Setup projection matrix as an orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof32(0, 256, 192, 0, 1024, -1024);

    // Setup modelview matrix as an identity matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Disable transparency and culling for new polygons
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

    // Setup background as transparent, and set it at the maximum distance
    glClearColor(0, 0, 0, 0);
    glClearDepth(0x7FFF);

    // Set the default vertex color as white
    glColor(RGB15(31, 31, 31));

    // Enable textures and alpha blending
    glEnable(GL_TEXTURE_2D | GL_BLEND);

    // 3D output is sent to layer 0, so enable it
    NF_ShowBg(0, 0);
}

u32 NF_GetTextureSize(u32 textel)
{
    // Return the texel size as a base 2 log: Real size = 8 << Returned size
    switch (textel)
    {
        case 8:
            return 0;
        case 16:
            return 1;
        case 32:
            return 2;
        case 64:
            return 3;
        case 128:
            return 4;
        case 256:
            return 5;
        case 512:
            return 6;
        case 1024:
            return 7;
        default:
            return 255;
    }
}
