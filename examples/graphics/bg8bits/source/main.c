/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de BackBuffer en 8bits (modo BITMAP)

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
	NF_InitBitmapBgSys(0, 0);
	NF_InitBitmapBgSys(1, 0);

	// Inicializa los buffers para guardar fondos en formato BITMAP
	NF_Init8bitsBgBuffers();

	// Inicializa el BackBuffer
	NF_Init8bitsBackBuffer(0);
	NF_Init8bitsBackBuffer(1);

	// Habilita el BackBuffer en la pantalla inferior
	NF_Enable8bitsBackBuffer(0);
	NF_Enable8bitsBackBuffer(1);

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load8bitsBg("bmp/img8b_1", 0);
	NF_Load8bitsBg("bmp/img8b_2", 1);
	NF_Load8bitsBg("bmp/img8b_3", 2);

	// Tranfiere las imagenes a la pantalla superior, backbuffer
	NF_Copy8bitsBuffer(0, 2, 1);
	// Tranfiere las imagenes a la pantalla superior, capa 3
	NF_Copy8bitsBuffer(0, 1, 0);
	// Transfiere la imagen a la pantalla inferior, capa 3
	NF_Copy8bitsBuffer(1, 1, 2);

	// Copia la paleta de la pantalla inferior al backbuffer
	u16 n = 0;
	for (n = 0; n < 256; n ++) {
		NF_8BITS_BACKBUFFER[1].pal[n] = *((u16*)(0x05000400 + (n << 1)));
	}

	// Se dibujara en color rojo
	NF_8BITS_BACKBUFFER[1].pal[255] = RGB15(31, 0, 0);

	// Variables del control del touchpad
	u16 keys = 0;
	touchPosition touchscreen;
	s16 x = 0;
	s16 y = 0;

	// Control del dibujado
	u16 idx = 0;
	u16 dx = 0;
	u16 dy = 0;

	// Control de fondo
	bool show = true;


	// Repite para siempre
	while (1) {

		// Lectura de posicion del stylus
		scanKeys();				// Lee el touchpad via Libnds
		touchRead(&touchscreen); 
		keys = keysHeld();		// Verifica el estado del touchscreen

		// Si presionas sobre el keypad...
		if (keys & KEY_TOUCH) {
			
			// Lee el touchpad
			x = (touchscreen.px);
			if (x > 247) x = 247;
			y = (touchscreen.py);
			if (y > 183) y = 183;

			// Dibuja un pixel de 8x8
			for (dy = 0; dy < 9; dy ++) {
				for (dx = 0; dx < 9; dx ++) {
					idx = ((y + dy) << 8) + (x + dx);
					// Pantalla superior
					NF_8BITS_BACKBUFFER[0].data[idx] = 0;
					// Pantalla inferior
					NF_8BITS_BACKBUFFER[1].data[idx] = 255;
				}
			}

		}


		// Lee las teclas pulsadas
		keys = keysDown();

		// Presiona A para escoder/mostrar capas 3 pantalla inferior
		if (keys & KEY_A) {
			show = !show;
			if (show) {
				// Cambia el valor del color transparente a ROSA
				NF_8BITS_BACKBUFFER[1].pal[0] = RGB15(31, 0, 31);
				// Muestra la capa 3 de la pantalla inferior
				NF_ShowBg(1, 3);
			} else {
				// Cambia el valor del color transparente a NEGRO
				NF_8BITS_BACKBUFFER[1].pal[0] = RGB15(0, 0, 0);
				// Esconde la capa 3 de la pantalla inferior
				NF_HideBg(1, 3);
			}
		}

		// Presiona X restaura imagen superior
		if (keys & KEY_X) NF_Copy8bitsBuffer(0, 2, 1);

		// Presiona B para borrar la capa de dibujado
		if (keys & KEY_B) memset(NF_8BITS_BACKBUFFER[1].data, 0, 65536);

		// Copia el BackBuffer de la pantalla superior a la capa 2
		NF_Flip8bitsBackBuffer(0, 0);
		// Copia el BackBuffer de la pantalla inferior a la capa 2
		NF_Flip8bitsBackBuffer(1, 0);

		// Sincronismo Vertical
		swiWaitForVBlank();

	}

	return 0; 

}
