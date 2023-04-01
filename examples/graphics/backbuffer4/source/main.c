/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de BackBuffer en 16bits (modo BITMAP)
	Zoom de imagen X2

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
	printf("\n NitroFS init. Please wait.\n\n");
	printf(" Iniciando NitroFS,\n por favor, espere.\n\n");
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

	// Inicializa el BackBuffer de 16 bits
	// Usalo solo una vez antes de habilitarlo
	NF_Init16bitsBackBuffer(0);
	NF_Init16bitsBackBuffer(1);

	// Habilita el backbuffer en la pantalla superior
	NF_Enable16bitsBackBuffer(0);
	NF_Enable16bitsBackBuffer(1);

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load16bitsBg("bmp/bitmap16", 0);

	// Tranfiere la imagen al BackBuffer de la pantalla inferior
	NF_Copy16bitsBuffer(1, 1, 0);

	// Variables RGB
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;

	// Calcula el valor RGB
	u16 rgb = 0;

	// Ventana
	s16 x = 0;
	s16 y = 0;
	s16 composite = 0;
	s16 square_x = 0;
	s16 square_y = 0;
	s16 zoom_x = 0;
	s16 zoom_y = 0;

	// Variables del control del touchpad
	u16 keys = 0;
	touchPosition touchscreen;

	while (1) {

		// Copia el original
		NF_Copy16bitsBuffer(1, 1, 0);

		// Lectura de posicion del stylus
		scanKeys();				// Lee el touchpad via Libnds
		touchRead(&touchscreen); 
		keys = keysHeld();		// Verifica el estado del touchscreen
		if (keys & KEY_TOUCH) {
			square_x = (touchscreen.px - 64);
			square_y = (touchscreen.py - 48);
		}

		// Calcula la ventana
		if (square_x < 0) square_x = 0;
		if (square_x > 127) square_x = 127;
		if (square_y < 0) square_y = 0;
		if (square_y > 95) square_y = 95;

		// Resetea el control de zoom
		zoom_x = 0;
		zoom_y = 0;
	
		// Rellena el buffer
		for (y = square_y; y < (square_y + 96); y ++) {
			for (x = square_x; x < (square_x + 128); x ++) {
				// Obten el color actual
				rgb = NF_16BITS_BACKBUFFER[1][((y << 8) + x)];
				// Escribe los pixeles en el Backbuffer de la pantalla superior 
				// Esto genera el Zoom x2 en la pantalla superior
				NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + zoom_x)] = rgb;
				NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + (zoom_x + 1))] = rgb;
				NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + zoom_x)] = rgb;
				NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + (zoom_x + 1))] = rgb;
				// Desglosa el RGB
				r = (rgb & 0x1F);
				g = ((rgb >> 5) & 0x1F);
				b = ((rgb >> 10) & 0x1F);
				// Genera el equivalente a blanco y negro y sube el brillo 3 puntos
				composite = ((int)((r + g + b) / 3) + 3);
				if (composite > 31) composite = 31;
				// Reemplaza los valores RGB
				r = (composite);
				g = (composite >> 2);
				b = (composite >> 1);
				// Calcula el valor RGB (Alpha + RGB555)
				rgb = ((r)|((g) << 5)|((b) << 10)|(BIT(15)));
				// Escribelos en el buffer
				NF_16BITS_BACKBUFFER[1][((y << 8) + x)] = rgb;
				// Incrementa dos puntos la X del zoom
				zoom_x += 2;
			}
			zoom_x = 0;		// Resetea la X del zoom
			zoom_y += 2;	// Incrementa dos puntos la Y del zoom
		}

		// Sincronismo Vertical
		swiWaitForVBlank();

		// Copia el contenido del Backbuffer a la VRAM
		NF_Flip16bitsBackBuffer(0);
		NF_Flip16bitsBackBuffer(1);

	}


	return 0; 

}
