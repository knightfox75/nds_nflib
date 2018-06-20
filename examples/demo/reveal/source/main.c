/*
-------------------------------------------------

	NightFox's Lib Template
	Efecto de "revelado"

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

	// Inicializa el motor 2D
	NF_Set2D(0, 0);		// Modo 2D_0 en la pantalla superior
	NF_Set2D(1, 5);		// Modo 2D_5 en la pantalla superior

	// Inicializa los fondos tileados (Pantalla superior)
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior

	// Inicializa los fondos en modo "BITMAP" (Pantalla inferior)
	NF_Init8bitsBgBuffers();		// Inicializa los buffers para almacenar fondos
	NF_InitBitmapBgSys(1, 0);		// Inicializa los fondos bitmap de 8 bits
	NF_Init8bitsBackBuffer(1);		// Inicializa el BackBuffer	
	NF_Enable8bitsBackBuffer(1);	// Habilita el BackBuffer en la pantalla inferior

	// Inicializa los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(1);		// Inicializa los sprites para la pantalla inferior

	// Carga los archivos de tiles desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load8bitsBg("bg/img8b_1", 0);
	NF_Load8bitsBg("bg/img8b_2", 1);

	// Carga los archivos de sprites desde la FAT / NitroFS a la RAM
	NF_LoadSpriteGfx("spr/bola", 0, 32, 32);			// Bola azul
	NF_LoadSpritePal("spr/bola", 0);

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");

	// Tranfiere las imagenes a la pantalla superior, backbuffer
	NF_Copy8bitsBuffer(1, 2, 1);
	// Tranfiere las imagenes a la pantalla superior, capa 3
	NF_Copy8bitsBuffer(1, 1, 0);
	// Copia el BackBuffer de la pantalla superior a la capa 2
	NF_Flip8bitsBackBuffer(1, 0);

	// Transfiere a la VRAM los sprites necesarios
	NF_VramSpriteGfx(1, 0, 0, true);	// Bola (todos los frames en VRAM) Pantalla superior
	NF_VramSpritePal(1, 0, 0);

	// Variables generales y inicializacion del random
	u8 n = 0;
	srand(time(NULL));

	// Crea las bolas en la pantalla superior
	s16 bola_x[32];
	s16 bola_y[32];
	s8 bola_spx[32];
	s8 bola_spy[32];
	for (n = 0; n < 32; n ++) {
		bola_x[n] = (rand() % 223);
		bola_y[n] = (rand() % 159);
		bola_spx[n] = (rand() % 3) + 1;
		bola_spy[n] = (rand() % 3) + 1;
		NF_CreateSprite(1, n, 0, 0, bola_x[n], bola_y[n]);
		NF_SpriteLayer(1, n, 3);
	}

	// Variables del control del touchpad
	u16 keys = 0;
	touchPosition touchscreen;
	s16 x = 0;
	s16 y = 0;

	// Control del dibujado
	u16 idx = 0;
	u16 dx = 0;
	u16 dy = 0;

	// Repite para siempre
	while (1) {

		// Lectura de posicion del stylus
		scanKeys();				// Lee el touchpad via Libnds
		touchRead(&touchscreen); 
		keys = keysHeld();		// Verifica el estado del touchscreen

		// Si presionas sobre el keypad...
		if (keys & KEY_TOUCH) {
			
			// Lee el touchpad
			x = (touchscreen.px - 8);
			if (x < 0) x = 0;
			if (x > 239) x = 239;
			y = ((touchscreen.py - 8));
			if (y < 0) y = 0;
			if (y > 175) y = 175;

			// Borra un bloque de 16x16
			for (dy = 0; dy < 16; dy ++) {
				for (dx = 0; dx < 16; dx ++) {
					idx = ((y + dy) << 8) + (x + dx);
					// Pantalla inferior
					NF_8BITS_BACKBUFFER[1].data[idx] = 0;
				}
			}

		}

		// Lee las teclas pulsadas
		keys = keysDown();

		// Presiona X restaura imagen superior
		if (keys & KEY_B) NF_Copy8bitsBuffer(1, 2, 1);

		// Copia el BackBuffer de la pantalla inferior a la capa 2
		NF_Flip8bitsBackBuffer(1, 0);

		// Mueve las bolas
		for (n = 0; n < 32; n ++) {
			bola_x[n] += bola_spx[n];
			if ((bola_x[n] < 0) || (bola_x[n] > 223)) bola_spx[n] *= -1;
			bola_y[n] += bola_spy[n];
			if ((bola_y[n] < 0) || (bola_y[n] > 159)) bola_spy[n] *= -1;
			NF_MoveSprite(1, n, bola_x[n], bola_y[n]);
		}

		// Actualiza el array de OAM
		NF_SpriteOamSet(1);

		swiWaitForVBlank();		// Espera al sincronismo vertical

		// Actualiza el OAM
		oamUpdate(&oamSub);

	}

	return 0; 

}
