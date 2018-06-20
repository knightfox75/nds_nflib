/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico modo mixto para fondos (tiled + bitmap)

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
#include <time.h>

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

	// Inicializa los fondos en modo Mixto
	// Capas 0 - 2 tiled // Capa 3 bitmap 8 bits
	NF_InitMixedBgSys(0);
	NF_InitMixedBgSys(1);

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos

	// Inicializa el motor de texto (requiere tener los fondos tileados inicializados)
	NF_InitTextSys(0);			// Inicializa el texto para la pantalla inferior

	// Inicializa los buffers para guardar fondos en formato BITMAP
	NF_Init8bitsBgBuffers();

	// Inicializa los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(0);		// Inicializa los sprites para la pantalla superior
	NF_InitSpriteSys(1);		// Inicializa los sprites para la pantalla inferior

	// Carga el fondo para la capa 0, pantalla inferior
	NF_LoadTiledBg("bg/bg0", "tiled_bg", 1024, 256);

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont16("fnt/font16", "text_bg0", 256, 256, 0);
	NF_LoadTextFont16("fnt/font16", "text_bg1", 256, 256, 0);

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load8bitsBg("bmp/img8b_1", 0);
	NF_Load8bitsBg("bmp/img8b_2", 1);

	NF_LoadSpriteGfx("sprite/bola", 0, 32, 32);			// Bola azul
	NF_LoadSpritePal("sprite/bola", 0);

	// Crea el fondo tileado en la capa inferior
	NF_CreateTiledBg(1, 0, "tiled_bg");

	// Crea las capas de texto
	NF_CreateTextLayer16(0, 0, 0, "text_bg0");
	NF_CreateTextLayer16(0, 1, 0, "text_bg1");
	// Define el color negro para texto de la capa 1
	NF_DefineTextColor(0, 1, 1, 0, 0, 0);		// Negro
	// Escribe un texto en la pantalla superior
	NF_WriteText16(0, 0, 1, 1, "Text over bitmap");
	NF_WriteText16(0, 0, 1, 3, "Text on layers 0 & 1");
	NF_WriteText16(0, 0, 1, 4, "Bitmap on layer 3");
	NF_ScrollBg(0, 0, 1, 1);
	// Y escribe un texto a modo de sombra
	NF_SetTextColor(0, 1, 1);
	NF_WriteText16(0, 1, 1, 1, "Text over bitmap");
	NF_WriteText16(0, 1, 1, 3, "Text on layers 0 & 1");
	NF_WriteText16(0, 1, 1, 4, "Bitmap on layer 3");
	// Actualiza las capas de texto
	NF_UpdateTextLayers();

	// Tranfiere las imagenes a la pantalla superior, capa 3
	NF_Copy8bitsBuffer(0, 1, 0);
	// Transfiere la imagen a la pantalla inferior, capa 3
	NF_Copy8bitsBuffer(1, 1, 1);

	// Transfiere a la VRAM los sprites necesarios
	NF_VramSpriteGfx(0, 0, 0, true);	// Bola, manten los frames adicionales en RAM
	NF_VramSpritePal(0, 0, 0);
	NF_VramSpriteGfx(1, 0, 0, true);
	NF_VramSpritePal(1, 0, 0);

	// Variables generales y inicializacion del random
	u8 n = 0;
	srand(time(NULL));

	// Crea las bolas en ambas pantallas
	s16 bola_x[32];
	s16 bola_y[32];
	s8 bola_spx[32];
	s8 bola_spy[32];
	for (n = 0; n < 32; n ++) {
		bola_x[n] = (rand() % 223);			// Calcula posiciones
		bola_y[n] = (rand() % 159);
		bola_spx[n] = (rand() % 3) + 1;
		bola_spy[n] = (rand() % 3) + 1;
		NF_CreateSprite(0, n, 0, 0, bola_x[n], bola_y[n]);	// Crea sprites
		NF_CreateSprite(1, n, 0, 0, bola_x[n], bola_y[n]);
		NF_SpriteLayer(0, n, 3);		// Se dibujaran sobre la capa 3
		NF_SpriteLayer(1, n, 3);
	}

	// Repite para siempre
	while (1) {

		// Mueve las bolas
		for (n = 0; n < 32; n ++) {
			bola_x[n] += bola_spx[n];
			if ((bola_x[n] < 0) || (bola_x[n] > 223)) bola_spx[n] *= -1;
			bola_y[n] += bola_spy[n];
			if ((bola_y[n] < 0) || (bola_y[n] > 159)) bola_spy[n] *= -1;
			NF_MoveSprite(0, n, bola_x[n], bola_y[n]);
			NF_MoveSprite(1, n, bola_x[n], bola_y[n]);
		}

		// Actualiza el array de OAM
		NF_SpriteOamSet(0);
		NF_SpriteOamSet(1);

		swiWaitForVBlank();		// Espera al sincronismo vertical

		// Actualiza el OAM
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);

	}

	return 0; 

}
