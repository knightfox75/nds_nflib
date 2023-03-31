// SPDX-License-Identifier: CC-BY-NC-4.0
//
// Copyright (c) 2011-2014 Cesar Rincon "NightFox"
//
// NightFox LIB - Include de funciones de carga de archivos multimedia
// http://www.nightfoxandco.com/
// Version 20140413

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_MEDIA_H__
#define __NF_MEDIA_H__

#include <nds.h>





// Funcion NF_LoadBMP();
void NF_LoadBMP(const char* file, u8 slot);
// Carga un archivo BMP de 8, 16 o 24 bits en un slot de imagenes de 16 bits.
// Debes inicializar el modo 16 bits, el backbuffer y usar la funcion NF_Draw16bitsImage();
// para mostrarlo.





#endif

#ifdef __cplusplus
}
#endif
