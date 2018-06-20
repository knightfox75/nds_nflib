/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de carga de fondos

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


// Algunos defines interesantes
#define SPEED 2		// Velocidad del scroll





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

	// Carga los archivos de fondo desde la FAT/NitroFS
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);			// Carga el fondo para la pantalla superior
	NF_LoadTiledBg("bg/bg3", "capa_3", 256, 256);		// Carga el fondo para la capa 3, pantalla inferior
	NF_LoadTiledBg("bg/bg2", "capa_2", 1024, 256);		// Carga el fondo para la capa 2, pantalla inferior
	NF_LoadTiledBg("bg/bg1", "capa_1", 1536, 256);		// Carga el fondo para la capa 1, pantalla inferior
	NF_LoadTiledBg("bg/bg0", "capa_0", 2048, 256);		// Carga el fondo para la capa 0, pantalla inferior


	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "capa_3");
	NF_CreateTiledBg(1, 2, "capa_2");
	NF_CreateTiledBg(1, 1, "capa_1");
	NF_CreateTiledBg(1, 0, "capa_0");


	// Variables
	s16 bg_x[4];
	s16 bg_y[4];

	u16 keys = 0;
	u8 n = 0;

	// Inicializa las variables
	for (n = 0; n < 4; n ++) {
		bg_x[n] = 0;
		bg_y[n] = 0;
	}



	// Bucle (repite para siempre)
	while(1) {

		// Lee el keypad
		scanKeys();
		keys = keysHeld();		// Teclas presionadas

		// Si pulsas izquierda
		if (keys & KEY_LEFT) {
			bg_x[0] -= SPEED;
			if (bg_x[0] < 0) bg_x[0] = 0;
		}

		// Si pulsas derecha
		if (keys & KEY_RIGHT) {
			bg_x[0] += SPEED;
			if (bg_x[0] > 1663) bg_x[0] = 1663;		// Limite fisico de la capa 0
		}

		// Calcula el efecto scroll parallax
		bg_x[1] = (int)(bg_x[0] / 1.5);
		bg_x[2] = (int)(bg_x[1] / 1.5);

		swiWaitForVBlank();		// Espera al sincronismo vertical

		// Actualiza la posicion de los fondos
		for (n = 0; n < 4; n ++) {
			NF_ScrollBg(1, n, bg_x[n], bg_y[n]);
		}
	
	}

	return 0; 

}
