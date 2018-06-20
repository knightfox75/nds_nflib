/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de fondos de colisiones

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
	NF_Set2D(0, 0);				// Modo 2D_0 en la pantalla superior

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior

	// Inicializa los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(0);		// Inicializa los sprites para la pantalla superior

	// Inicializa los buffers de mapas de colisiones
	NF_InitCmapBuffers();

	// Carga los archivos de fondo
	NF_LoadTiledBg("bg/ppc_bg", "bg3", 512, 512);		// Carga el fondo para la capa 3, pantalla superior

	// Carga los archivos de sprites
	NF_LoadSpriteGfx("sprite/puntero", 0, 8, 8);	// Puntero
	NF_LoadSpritePal("sprite/puntero", 0);

	// Carga el fondo de colisiones
	NF_LoadColisionBg("maps/ppc_cmap", 0, 512, 512);

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "bg3");

	// Transfiere a la VRAM los sprites necesarios
	NF_VramSpriteGfx(0, 0, 0, true);	// Puntero
	NF_VramSpritePal(0, 0, 0);

	// Crea el Sprite del puntero en la pantalla inferior
	NF_CreateSprite(0, 0, 0, 0, 124, 92);	// Crea el puntero en la pantalla inferior
	NF_SpriteLayer(0, 0, 3);				// Y la capa sobre la que se dibujara
	
	// Variable para la lectura del keypad
	u16 keys = 0;

	// Variables para el control de movimiento
	s16 x = 128;
	s16 y = 96;
	s16 spr_x = 0;
	s16 spr_y = 0;
	s16 bg_x = 0;
	s16 bg_y = 0;
	u8 pixel = 0;

	// Bucle (repite para siempre)
	while(1) {

		// Lee el teclado
		scanKeys();
		keys = keysHeld();
		if (keys & KEY_UP) y --;
		if (keys & KEY_DOWN) y ++;
		if (keys & KEY_LEFT) x --;
		if (keys & KEY_RIGHT) x ++;

		// Limites del movimiento
		if (x < 0) x = 0;
		if (x > 511) x = 511;
		if (y < 0) y = 0;
		if (y > 511) y = 511;

		// Posicion del fondo
		bg_x = (x - 128);
		if (bg_x < 0) bg_x = 0;
		if (bg_x > 256) bg_x = 256;
		bg_y = (y - 96);
		if (bg_y < 0) bg_y = 0;
		if (bg_y > 320) bg_y = 320;

		// Posicion del Sprite
		spr_x = (x - bg_x) - 4;
		spr_y = (y - bg_y) - 4;
		NF_MoveSprite(0, 0, spr_x, spr_y);

		// Imprime la posicion del cursor
		consoleClear();
		iprintf("x:%03d  y:%03d \n\n", x, y);

		// Imprime el color del pixel
		pixel = NF_GetPoint(0, x, y);
		switch (pixel) {
			case 1:
				iprintf("Tile: Negro / Black");
				break;
			case 2:
				iprintf("Tile: Rojo / Red");
				break;
			case 3:
				iprintf("Tile: Verde / Green");
				break;
			case 4:
				iprintf("Tile: Azul / Blue");
				break;
			default:
				iprintf("Value: %03d", pixel); 
				break;
		}

		NF_SpriteOamSet(0);				// Actualiza el Array del OAM

		swiWaitForVBlank();				// Espera al sincronismo vertical

		oamUpdate(&oamMain);			// Actualiza a VRAM el OAM Secundario

		NF_ScrollBg(0, 3, bg_x, bg_y);	// Actualiza el scroll 
	
	}

	return 0; 

}
