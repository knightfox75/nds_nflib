/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de Hflip y Vflip de tiles del fondo

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


// Variables
u16 mapa[32][24];	// Almacena el mapa (32x24 tiles)





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

	// Inicializa el motor 2D
	NF_Set2D(0, 0);				// Modo 2D_0 en ambas pantallas
	NF_Set2D(1, 0);

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior
	NF_InitTiledBgSys(1);		// Iniciliaza los fondos Tileados para la pantalla inferior

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);			// Carga el fondo para la pantalla inferior

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");

	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "nfl");

	// Invierte todos los tiles del mapa de la pantalla inferior y copia los tiles al buffer temporal
	u8 x = 0;
	u8 y = 0;
	for (y = 0; y < 24; y ++) {
		for (x = 0; x < 32; x ++) {
			NF_SetTileHflip(1, 3, x, y);
			NF_SetTileVflip(1, 3, x, y);	// *** Comenta esta linea para el efecto espejo
			mapa[x][y] = NF_GetTileOfMap(1, 3, x, y);
		}
	}
	// Ahora coloca los tiles en orden inverso para compensar la imagen
	for (y = 0; y < 24; y ++) {
		for (x = 0; x < 32; x ++) {
			NF_SetTileOfMap(1, 3, (31 - x), (23 - y), mapa[x][y]);	// *** Usa esta linea para el efecto rotado 180º
			// NF_SetTileOfMap(1, 3, (31 - x), y, mapa[x][y]);		// *** Usa esta linea para el efecto espejo
		}
	}
	// Y para finalizar, actualiza el mapa en la VRAM
	NF_UpdateVramMap(1, 3);


	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();		// Espera al sincronismo vertical
	
	}

	return 0; 

}
