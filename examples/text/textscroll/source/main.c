/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de texto con scroll

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

	// Inicializa el motor de texto (requiere tener los fondos tileados inicializados)
	NF_InitTextSys(0);			// Inicializa el texto para la pantalla superior
	NF_InitTextSys(1);			// Inicializa el texto para la pantalla inferior

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/layer3", "moon", 256, 256);		// Carga el fondo para la capa 3, pantalla inferior

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont("fnt/default", "up", 256, 768, 0);	// Carga la seccion "normal" de la fuente, tamaño del mapa 256x256
	NF_LoadTextFont("fnt/default", "down", 768, 256, 2);	// Carga la seccion "normal" de la fuente, tamaño del mapa 256x256

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "moon");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "moon");

	// Crea una capa de texto
	NF_CreateTextLayer(0, 0, 0,	"up");
	NF_CreateTextLayer(1, 0, 2,	"down");

	// Variables
	u16 n = 0;
	char mytext[32];
	u16 keys;
	s16 x = 0;
	s16 y = 0;
	s16 contador = 0;
	s8 delay = 0;

	// Escribe 96 lineas de texto
	for (n = 0; n < 96; n ++) {
		sprintf(mytext," Linea: %02d", n);
		NF_WriteText(0, 0, 0, n, mytext);
		NF_WriteText(1, 0, 0, n, mytext);
	}

	// Actualiza las capas de texto
	NF_UpdateTextLayers();


	// Bucle (repite para siempre)
	while(1) {

		delay ++;
		if (delay > 10) {

			delay = 0;

			// Escribe 96 lineas de texto
			for (n = 0; n < 96; n ++) {
				sprintf(mytext,"%04d", contador);
				NF_WriteText(0, 0, 26, n, mytext);
				NF_WriteText(1, 0, 18, n, mytext);
				contador ++;
				if (contador > 9999) contador = 0;
			}

			// Actualiza las capas de texto
			NF_UpdateTextLayers();

		}

		// Lee el teclado
		scanKeys();
		keys = keysHeld();

		// Scroll del texto pantalla superior
		if (keys & KEY_UP) y -= 2;
		if (keys & KEY_DOWN) y += 2;
		if (y < 0) y = 0;
		if (y > 575) y = 575;

		// Scroll del texto de la pantalla inferior
		if (keys & KEY_LEFT) x -= 2;
		if (keys & KEY_RIGHT) x += 2;
		if (x < 0) x = 0;
		if (x > 511) x = 511;

		swiWaitForVBlank();			// Espera al sincronismo vertical

		// Mueve la capa de texto
		NF_ScrollBg(0, 0, 0, y);
		NF_ScrollBg(1, 0, x, 63);
	
	}

	return 0; 

}
