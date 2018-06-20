/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de texto con paletas extendidas

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
	NF_InitTextSys(1);			// Inicializa el texto para la pantalla inferior

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);			// Carga el fondo para la pantalla inferior

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont("fnt/default", "down", 256, 256, 0);

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");

	// Crea una capa de texto
	NF_CreateTextLayer(1, 0, 0,	"down");

	// Define algunos colores para el texto (pantalla inferior, capa 3)
	NF_DefineTextColor(1, 0, 1, 31, 0, 0);		// Rojo
	NF_DefineTextColor(1, 0, 2, 0, 31, 0);		// Verde
	NF_DefineTextColor(1, 0, 3, 0, 0, 31);		// Azul

	// Ahora algunos colores aleatorios
	srand(time(NULL));
	u8 n = 0;
	for (n = 4; n < 16; n ++) {
		NF_DefineTextColor(1, 0, n, ((rand() % 31) + 1), ((rand() % 31) + 1), ((rand() % 31)) + 1);
	}

	// Texto
	NF_WriteText(1, 0, 1, 1, "Defecto / Default");	// Texto con el color por defecto de la fuente
	NF_SetTextColor(1, 0, 1);						// Selecciona el color del texto a escribir
	NF_WriteText(1, 0, 1, 2, "Rojo / Red");			// y escribe el texto en ese color
	NF_SetTextColor(1, 0, 2);
	NF_WriteText(1, 0, 1, 3, "Verde / Green");
	NF_SetTextColor(1, 0, 3);
	NF_WriteText(1, 0, 1, 4, "Azul / Blue");
	// Texto con color aleatorio
	for (n = 4; n < 16; n ++) {
		NF_SetTextColor(1, 0, n);
		NF_WriteText(1, 0, 1, (n + 1), "Random color");
	}
	NF_SetTextColor(1, 0, 0);					// Deja color del texto por defecto (definido en la fuente)

	// Actualiza las capas de texto
	NF_UpdateTextLayers();

	// Variables
	u8 speed = 0;

	// Bucle (repite para siempre)
	while(1) {

		speed ++;
		if (speed > 60) {
			speed = 0;
			// Calcula un nuevo set de paletas
			for (n = 4; n < 16; n ++) {
				NF_DefineTextColor(1, 0, n, ((rand() % 31) + 1), ((rand() % 31) + 1), ((rand() % 31)) + 1);
			}
			// Escribe de nuevo el texto con los nuevos colores
			for (n = 4; n < 16; n ++) {
				NF_SetTextColor(1, 0, n);
				NF_WriteText(1, 0, 1, (n + 1), "Random color");
			}
			// Actualiza las capas de texto
			NF_UpdateTextLayers();
		}

		swiWaitForVBlank();		// Espera al sincronismo vertical
	
	}

	return 0; 

}
