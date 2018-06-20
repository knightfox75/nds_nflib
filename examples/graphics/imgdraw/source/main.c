/*
-------------------------------------------------

	NightFox's Lib Template
	Dibujado de imagenes de 16 bits

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

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load16bitsBg("bmp/bitmap16", 0);
	NF_Load16bitsImage("bmp/blueball", 1, 64, 64);

	// Tranfiere la imagen a la VRAM de ambas pantallas
	NF_Copy16bitsBuffer(0, 1, 0);
	NF_Copy16bitsBuffer(1, 1, 0);

	// Variables
	s16 x = 0;
	s16 y = 0;
	s8 ix = 1;
	s8 iy = 1;

	// Repite para siempre
	while (1) {

		// Calcula el movimiento
		x += ix;
		if ((x <= 0) || (x >= 191)) ix *= -1;
		y += iy;
		if ((y <= 0) || (y >= 127)) iy *= -1;

		// Redibuja la escena (Superior)
		NF_Draw16bitsImage(0, 1, x, y, true);		// Bola (con magenta como transparente)

		// Redibuja la escena (Inferior)
		NF_Copy16bitsBuffer(1, 1, 0);				// Fondo
		NF_Draw16bitsImage(1, 1, x, y, true);		// Bola (con magenta como transparente)

		// Sincronismo Vertical
		swiWaitForVBlank();

		// Manda el backbuffer a la pantalla
		NF_Flip16bitsBackBuffer(0);
		NF_Flip16bitsBackBuffer(1);

	}


	return 0; 

}
