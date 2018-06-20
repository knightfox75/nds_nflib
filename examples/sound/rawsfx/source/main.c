/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de sonidos en formato RAW

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

	// Inicializa el engine de audio de la DS
	soundEnable();

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior
	NF_InitTiledBgSys(1);		// Iniciliaza los fondos Tileados para la pantalla inferior

	// Inicializa el motor de texto
	NF_InitTextSys(0);			// Inicializa el texto para la pantalla superior

	// Inicializa los buffers de sonido
	NF_InitRawSoundBuffers();

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/layer3", "moon", 256, 256);		// Carga el fondo para la capa 3, pantalla inferior

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);	// Carga la seccion "normal" de la fuente

	// Carga los samples de sonido en formato RAW
	NF_LoadRawSound("sfx/sample", 0, 11025, 0);
	NF_LoadRawSound("sfx/music", 1, 22050, 0);

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "moon");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "moon");

	// Crea una capa de texto
	NF_CreateTextLayer(0, 0, 0,	"normal");

	// Escribe un texto en cada capa de texto
	NF_WriteText(0, 0, 1, 1, "L or R play voice.");
	NF_WriteText(0, 0, 1, 2, "A or B STOP/PLAY music.");
	NF_WriteText(0, 0, 1, 4, "L o R reproducir una voz.");
	NF_WriteText(0, 0, 1, 5, "A or B musica STOP/PLAY");

	// Actualiza las capas de texto
	NF_UpdateTextLayers();

	// Variables
	u16 newpress = 0;
	u8 sound_id = 0;

	// Inicia la musica de fondo
	sound_id = NF_PlayRawSound(1, 127, 64, true, 0);

	// Bucle (repite para siempre)
	while(1) {

		// Lee el teclado
		scanKeys();
		newpress = keysDown();

		// Detiene el sonido, pero no la borra de la memoria de sonido
		if (newpress & KEY_A) soundPause(sound_id);
		// Vuelve a reproducir el sonido, pero desde el principio
		if (newpress & KEY_B) soundResume(sound_id);
		// Para eliminar el sonido de la memoria de sonido (no del buffer de la libreria)
		// usa la funcion soundKill();

		// Reproduce el sonido
		if (newpress & KEY_R) NF_PlayRawSound(0, 127, 127, false, 0);
		if (newpress & KEY_L) NF_PlayRawSound(0, 127, 0, false, 0);

		swiWaitForVBlank();			// Espera al sincronismo vertical
	
	}

	return 0; 

}
