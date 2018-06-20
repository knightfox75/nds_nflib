/*
-------------------------------------------------

	NightFox's Lib Template
	Carga de archivos BMP y Scroll

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


#define SPEED 3


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
	NF_LoadBMP("bmp/lostend", 0);

	// Variables
	u16 keys = 0;
	s16 x = 0;
	s16 y = 0;
	s16 scr_x = 0;
	s16 scr_y = 0;
	s16 right = 0;
	s16 bottom = 0;
	u16 mini_x = 0;
	u16 mini_y = 0;
	u16 desp_x = 0;
	u16 desp_y = 0;
	u32 scr_idx = 0;
	u32 img_idx = 0;

	// Calcula los limites de dibujado
	if (NF_BG16B[0].width > 256) {
		right = 256;
	} else {
		right = NF_BG16B[0].width;
	}
	if (NF_BG16B[0].height > 192) {
		bottom = 192;
	} else {
		bottom = NF_BG16B[0].height;
	}

	// Calcula el tamaño de la miniatura y el desplazamiento para posicionarla en pantalla
	if (NF_BG16B[0].width >= NF_BG16B[0].height) {	// Si la imagen es mas ancha que alta
		mini_x = right;
		mini_y = (int)((NF_BG16B[0].height * right) / NF_BG16B[0].width);
		desp_y = (int)((192 - mini_y) >> 1);
	} else {		// Si es mas alta que ancha
		mini_x = (int)((NF_BG16B[0].width * bottom) / NF_BG16B[0].height);
		mini_y = bottom;
		desp_x = (int)((256 - mini_x) >> 1);
	}

	// Genera la miniatura
	for (scr_y = 0; scr_y < mini_y; scr_y ++) {
		for (scr_x = 0; scr_x < mini_x; scr_x ++ ) {
			// Calcula los offsets
			scr_idx = ((scr_y + desp_y) << 8) + (scr_x + desp_x);
			img_idx = (int)((((scr_y * NF_BG16B[0].height) / mini_y) * NF_BG16B[0].width) + ((scr_x * NF_BG16B[0].width) / mini_x));
			// Escribe los valores
			*(NF_16BITS_BACKBUFFER[1] + scr_idx) = NF_BG16B[0].buffer[img_idx];
		}
	}
	// Manda el backbuffer a la pantalla
	NF_Flip16bitsBackBuffer(1);

	// Repite para siempre
	while (1) {

		// Lee el teclado
		scanKeys();
		keys = keysHeld();

		// Lee el teclado
		if (NF_BG16B[0].width > 256) {
			if (keys & KEY_LEFT) {
				x -= SPEED;
				if (x < 0) x = 0;
			}
			if (keys & KEY_RIGHT) {
				x += SPEED;
				if (x > (NF_BG16B[0].width - 256)) x = (NF_BG16B[0].width - 256);
			}
		}
		if (NF_BG16B[0].height > 192) {
			if (keys & KEY_UP) {
				y -= SPEED;
				if (y < 0) y = 0;
			}
			if (keys & KEY_DOWN) {
				y += SPEED;
				if (y > (NF_BG16B[0].height - 192)) y = (NF_BG16B[0].height - 192);
			}
		}

		// Dibuja la imagen en el BackBuffer
		for (scr_y = 0; scr_y < bottom; scr_y ++) {
			for(scr_x = 0; scr_x < right; scr_x ++ ) {
				// Calcula los offsets
				scr_idx = (scr_y << 8) + scr_x;
				img_idx = (((scr_y + y) * NF_BG16B[0].width) + (scr_x + x));
				// Escribe los valores
				*(NF_16BITS_BACKBUFFER[0] + scr_idx) = NF_BG16B[0].buffer[img_idx];
			}
		}

		// Sincronismo Vertical
		swiWaitForVBlank();

		// Manda el backbuffer a la pantalla
		NF_Flip16bitsBackBuffer(0);

	}


	return 0; 

}
