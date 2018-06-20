/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo graficos en modo BITMAP de 16bits

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

	// Inicializa los fondos en BITMAP
	NF_InitBitmapBgSys(0, 1);
	NF_InitBitmapBgSys(1, 1);


	// Variables RGB
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;
	// Calcula el valor RGB
	u16 rgb = 0;
	// Direccion en VRAM
	u32 adress = 0;
	// Otras
	s16 x = 0;
	s16 y = 0;

	// Rellena la pantalla
	for (y = 0; y < 256; y ++) {
		for (x = 0; x < 256; x ++) {
			// Calcula el nuevo color
			r ++;
			if (r > 31) {
				r = 0;
				g ++;
				if (g > 31) {
					g = 0;
					b ++;
					if (b > 31) b = 0;
				}
			}
			// Calcula el valor RGB (Alpha + RGB555)
			rgb = ((r)|((g) << 5)|((b) << 10)|(BIT(15)));
			// Calcula la posicion donde se escribira en la VRAM (Banco A, modo bitmap, pantalla superior)
			adress = (0x06000000 + (((y << 8) + x) << 1));
			// Escribe el valor
			*((u16*)adress) = rgb;
			// Calcula la posicion donde se escribira en la VRAM (Banco C, modo bitmap, pantalla inferior)
			adress = (0x06200000 + (((x << 8) + y) << 1));
			// Escribe el valor
			*((u16*)adress) = rgb;
		}
	}


	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();			// Espera al sincronismo vertical
	
	}

	return 0; 

}
