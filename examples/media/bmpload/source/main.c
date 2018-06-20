/*
-------------------------------------------------

	NightFox's Lib Template
	Carga de archivos BMP de 8, 16 y 24 bits.

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por NightFox
	http://www.nightfoxandco.com
	Inicio 10 de Octubre del 2009

	(c) 2009 - 2011 NightFox & Co.

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


#define MAX_BMP_FILES 3





/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

int main(int argc, char **argv) {

	// Pantalla de espera inicializando NitroFS
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);	
	consoleDemoInit();
	iprintf("\n NitroFS init. Please wait.\n\n");
	iprintf(" Iniciando NitroFS,\n por favor, espere.\n\n");
	swiWaitForVBlank();

	// Define el ROOT e inicializa el sistema de archivos
	NF_SetRootFolder("NITROFS");	// Define la carpeta ROOT para usar NITROFS

	// Inicializa el motor 2D en modo BITMAP
	NF_Set2D(0, 5);				// Modo 2D_5 en ambas pantallas
	NF_Set2D(1, 5);

	// Inicializa los fondos en modo "BITMAP"
	NF_InitBitmapBgSys(0, 1);
	NF_InitBitmapBgSys(1, 1);

	// Inicializa los buffers para guardar fondos en formato BITMAP
	NF_Init16bitsBgBuffers();

	// Inicializa el backbuffer
	NF_Init16bitsBackBuffer(0);
	NF_Init16bitsBackBuffer(1);

	// Habilita el backbuffer
	NF_Enable16bitsBackBuffer(0);
	NF_Enable16bitsBackBuffer(1);

	// Carga los archivos BITMAP
	NF_LoadBMP("bmp/bmp8bits", 0);
	NF_LoadBMP("bmp/bmp16bits", 1);
	NF_LoadBMP("bmp/bmp24bits", 2);

	// Variables
	s8 img1 = 0;
	s8 img2 = 1;
	u16 keys = 0;

	// Dibuja las imagenes en los backbuffers
	NF_Draw16bitsImage(0, img1, 0, 0, false);
	NF_Draw16bitsImage(1, img2, 0, 0, false);

	// Repite para siempre
	while (1) {

		// Lee el teclado
		scanKeys();
		keys = keysDown();

		// Segun la tecla pulsada
		if ((keys & KEY_UP) || (keys & KEY_DOWN)) {
			if (keys & KEY_UP) {
				img1 --;
				img2 --;
				if (img1 < 0) img1 = (MAX_BMP_FILES - 1);
				if (img2 < 0) img2 = (MAX_BMP_FILES - 1);
			}
			if (keys & KEY_DOWN) {
				img1 ++;
				img2 ++;
				if (img1 > (MAX_BMP_FILES - 1)) img1 = 0;
				if (img2 > (MAX_BMP_FILES - 1)) img2 = 0;
			}
			// Dibuja las imagenes en los backbuffers
			NF_Draw16bitsImage(0, img1, 0, 0, false);
			NF_Draw16bitsImage(1, img2, 0, 0, false);
		}

		// Sincronismo Vertical
		swiWaitForVBlank();

		// Manda el backbuffer a la pantalla
		NF_Flip16bitsBackBuffer(0);
		NF_Flip16bitsBackBuffer(1);

	}


	return 0; 

}
