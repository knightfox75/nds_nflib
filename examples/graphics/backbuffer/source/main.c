/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de BackBuffer en 16bits (modo BITMAP)

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por NightFox
	http://www.nightfoxandco.com
	Inicio 10 de Octubre del 2009

	(c)2009 - 2011 NightFox & Co.

-------------------------------------------------
*/





/*
-------------------------------------------------
	Includes
-------------------------------------------------
*/

// Includes C
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Includes propietarios NDS
#include <nds.h>

// Includes librerias propias
#include <nf_lib.h>





/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

int main(int argc, char **argv) {

	// Inicializa el motor 2D en modo BITMAP
	NF_Set2D(0, 5);				// Modo 2D_5 en ambas pantallas
	NF_Set2D(1, 5);

	// Inicializa los fondos en modo "BITMAP"
	NF_InitBitmapBgSys(0, 1);
	NF_InitBitmapBgSys(1, 1);

	// Inicializa el BackBuffer de 16 bits en ambas pantallas
	// Usalo solo una vez antes de habilitarlo
	NF_Init16bitsBackBuffer(0);
	NF_Init16bitsBackBuffer(1);

	// Habilita el backbuffer de ambas pantallas
	NF_Enable16bitsBackBuffer(0);
	NF_Enable16bitsBackBuffer(1);


	// Variables RGB
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;
	// Calcula el valor RGB
	u16 rgb = 0;
	// Otras
	s16 x = 0;
	s16 y = 0;

	while (1) {
	
		// Rellena el buffer
		for (y = 0; y < 256; y ++) {
			for (x = 0; x < 256; x ++) {
				// Calcula el nuevo color
				r ++;
				if (r > 31) {
					r = 0;
					g ++;
					if (g > 16) {
						g = 0;
					}
				}
				b = ((y << 5) >> 8);
				// Calcula el valor RGB (Alpha + RGB555)
				rgb = ((r)|((g) << 5)|((b) << 10)|(BIT(15)));
				// Escribelos en el buffer
				NF_16BITS_BACKBUFFER[0][((y << 8) + x)] = rgb;
				NF_16BITS_BACKBUFFER[1][((x << 8) + y)] = rgb;
			}
		}

		// Copia el contenido del Backbuffer a la VRAM
		NF_Flip16bitsBackBuffer(0);
		NF_Flip16bitsBackBuffer(1);

		swiWaitForVBlank();

	}

	return 0; 

}
