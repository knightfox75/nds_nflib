/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de paletas extendidas para fondos

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
#include <time.h>

// Includes propietarios NDS
#include <nds.h>

// Includes librerias propias
#include <nf_lib.h>



// Algunos defines interesantes
#define RED 1
#define GREEN 2
#define BLUE 3





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
	NF_LoadTiledBg("bg/blue", "top", 256, 256);			// Carga el fondo para la pantalla inferior
	NF_LoadTiledBg("bg/blue", "bottom", 256, 256);			// Carga el fondo para la pantalla inferior
	// Carga las paletas extendidas
	NF_LoadExBgPal("bg/red", RED);			
	NF_LoadExBgPal("bg/green", GREEN);
	NF_LoadExBgPal("bg/blue", BLUE);

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont("fnt/default", "up", 256, 256, 0);	// Carga la seccion "normal" de la fuente, tamaño del mapa 256x256
	NF_LoadTextFont("fnt/default", "down", 256, 256, 0);

	// Tranfiere las paletas del fondo 3
	u8 n = 0;
	for (n = 1; n < 4; n ++) {
		NF_VramExBgPal(0, 3, n, n);
		NF_VramExBgPal(1, 3, n, n);
	}

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "top");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "bottom");

	// Crea una capa de texto
	NF_CreateTextLayer(0, 0, 0,	"up");
	NF_CreateTextLayer(1, 0, 0,	"down");

	// Variables
	u8 color = RED;
	u8 x = 0;
	u8 y = 0;
	char text[32];


	// Bucle (repite para siempre)
	while(1) {

		// Cambia el la paleta que usara este tile
		NF_SetTilePal(0, 3, x, y, color);

		// Texto
		sprintf(text, "X:%02d  Y:%02d   ExPal:%02d  ", x, y, color);
		NF_WriteText(0, 0, 1, 1, text);

		// Calcula el siguiente tile
		x ++;
		if (x > 31) {
			x = 0;
			y ++;
			if (y > 23) {
				y = 0;
				// Cambia la paleta extendida del fondo inferior
				NF_SetExBgPal(1, 3, color);
				// Texto
				sprintf(text, "ExPal:%02d  ", color);
				NF_WriteText(1, 0, 1, 1, text);
				// Y realiza un cambio de paleta si toca
				color ++;
				if (color > BLUE) color = RED;
			}
		}

		// Ahora actualiza el mapa en la VRAM
		NF_UpdateVramMap(0, 3);

		// Actualiza las capas de texto
		NF_UpdateTextLayers();

		swiWaitForVBlank();		// Espera al sincronismo vertical
	
	}

	return 0; 

}
