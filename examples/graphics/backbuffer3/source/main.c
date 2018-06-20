/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de BackBuffer en 16bits (modo BITMAP)
	con carga de archivos de imagen.
	Efecto de visualizar dos imagenes con efecto ventana.

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

	// Inicializa el BackBuffer de 16 bits
	// Usalo solo una vez antes de habilitarlo
	NF_Init16bitsBackBuffer(1);

	// Habilita el backbuffer en la pantalla superior
	NF_Enable16bitsBackBuffer(1);

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load16bitsBg("bmp/bitmap16", 0);
	NF_Load16bitsBg("bmp/img16_a", 1);
	NF_Load16bitsBg("bmp/img16_b", 2);

	// Tranfiere la imagen a la VRAM de ambas pantallas
	NF_Copy16bitsBuffer(0, 0, 0);
	NF_Copy16bitsBuffer(1, 1, 2);

	// Si no es necesario usarla mas, borrala de la RAM
	NF_Unload16bitsBg(0);

	// Variables del control del touchpad
	u16 keys = 0;
	touchPosition touchscreen;

	// Variables del control de la ventana
	u32 move = 0;
	s16 y = 0;
	s16 xa = 0;
	s16 xb = 0;
	s16 ya = 0;
	s16 yb = 0;
	s16 sqr_xa = 0;
	s16 sqr_xb = 0;
	s16 sqr_ya = 0;
	s16 sqr_yb = 0;
	bool in_touch = false;

	// Repite para siempre
	while (1) {

		// Copia al backbuffer la imagen de arriba
		NF_Copy16bitsBuffer(1, 1, 2);

		// Lectura de posicion del stylus
		scanKeys();					// Lee el touchpad via Libnds
		touchRead(&touchscreen); 
		keys = keysHeld();			// Verifica el estado del touchscreen
		if (keys & KEY_TOUCH) {		// Si se toca el touchpad
			if (!in_touch) {		// Y es el primer toque
				xa = touchscreen.px;	// Guarda el punto
				ya = touchscreen.py;
				in_touch = true;		// e indicalo
			}
				xb = touchscreen.px;	// Guarda el segundo punto
				yb = touchscreen.py;
		} else {
			in_touch = false;			// Ya no se esta tocando el touchpad
		}

		// Si se ha tocado el touchpad y la ventana es del tamaño suficiente...
		if (in_touch && (xa != xb) && (ya != yb)) {
			// Calcula los limites de la ventana
			if (xa < xb) {
				sqr_xa = xa;
				sqr_xb = xb;
			} else {
				sqr_xa = xb;
				sqr_xb = xa;
			}
			if (ya < yb) {
				sqr_ya = ya;
				sqr_yb = yb;
			} else {
				sqr_ya = yb;
				sqr_yb = ya;
			}
			// Ahora dibuja la ventana linea a linea
			for (y = sqr_ya; y < sqr_yb; y ++) {
				// Calcula donde se escribira la linea
				move = ((y << 8) + sqr_xa);
				// Copia la linea de la Imagen A sobre la B
				memcpy((NF_16BITS_BACKBUFFER[1] + move), (NF_BG16B[1].buffer + move), ((sqr_xb - sqr_xa) << 1));
			}
		}

		// Sincronismo Vertical
		swiWaitForVBlank();

		// Manda el backbuffer a la pantalla
		NF_Flip16bitsBackBuffer(1);

	}


	return 0; 

}
