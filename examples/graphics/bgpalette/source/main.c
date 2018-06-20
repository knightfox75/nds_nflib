/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de efectos de paletas

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

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/layer3", "moon", 256, 256);		// Carga el fondo para la capa 3, pantalla inferior

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);	// Carga la seccion "normal" de la fuente, tamaño del mapa 256x256
	NF_LoadTextFont("fnt/default", "right", 256, 256, 1);		// Carga la seccion "rotar derecha" de la fuente, tamaño del mapa 256x256
	NF_LoadTextFont("fnt/default", "left", 256, 256, 2);		// Carga la seccion "rotar izquierda· de la fuente, tamaño del mapa 256x256

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "moon");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "moon");

	// Crea una capa de texto
	NF_CreateTextLayer(0, 0, 0,	"normal");
	NF_CreateTextLayer(0, 1, 1, "right");
	NF_CreateTextLayer(0, 2, 2, "left");

	// Compensa el espacio "muerto" de la pantalla al rotar el texto a la izquierda
	// Desplazandolo 64 pixeles en vertical
	NF_ScrollBg(0, 2, 0, 63);

	// Escribe un texto en cada capa de texto
	NF_BgSetPalColor(0, 0, 1, 31, 31, 0);		// Color del texto
	NF_WriteText(0, 0, 1, 1, "Hola Mundo!\n Hello World!");	// Normal
	NF_BgEditPalColor(0, 1, 1, 0, 31, 31);	// Color del texto
	NF_BgUpdatePalette(0, 1);	
	NF_WriteText(0, 1, 1, 1, "Hola Mundo!\n Hello World!");	// Rotado derecha
	NF_WriteText(0, 2, 1, 1, "Hola Mundo!\n Hello World!");	// Rotado izquierda

	// Actualiza las capas de texto
	NF_UpdateTextLayers();

	// Variables
	char mytext[32];
	u32 myvar = 0;
	u8 r, g, b;
	s16 cR, cG, cB;
	u8 speed = 0;
	u16 n = 0;

	// Bucle (repite para siempre)
	while(1) {

		myvar ++;
		sprintf(mytext,"Contador: %d", myvar);
		NF_WriteText(0, 0, 1, 5, mytext);
		NF_WriteText(0, 1, 1, 5, mytext);
		NF_WriteText(0, 2, 1, 5, mytext);

		NF_UpdateTextLayers();		// Actualiza las capas de texto

		//	Efecto añochecer modificando la paleta
		speed ++;
		if (speed > 30) {
			speed = 0;
			for (n = 1; n < 256; n ++) {
				NF_BgGetPalColor(1, 3, n, &r, &g, &b);
				cR = r;
				cG = g;
				cB = b;
				cR --;
				if (cR < 0) cR = 0;
				cG --;
				if (cG < 0) cG = 0;
				NF_BgEditPalColor(1, 3, n, cR, cG, cB);
			}
			NF_BgUpdatePalette(1, 3);
		}
		//

		swiWaitForVBlank();			// Espera al sincronismo vertical
	
	}

	return 0; 

}
