/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de manipulacion de paletas de Sprites

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

	// Inicializa los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(0);		// Inicializa los sprites para la pantalla superior
	NF_InitSpriteSys(1);		// Inicializa los sprites para la pantalla inferior

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);			// Carga el fondo para la pantalla superior
	NF_LoadTiledBg("bg/bg3", "capa_3", 256, 256);		// Carga el fondo para la capa 3, pantalla inferior
	NF_LoadTiledBg("bg/bg2", "capa_2", 1024, 256);		// Carga el fondo para la capa 2, pantalla inferior


	// Carga los archivos de sprites desde la FAT / NitroFS a la RAM
	NF_LoadSpriteGfx("sprite/personaje", 0, 64, 64);	// Personaje
	NF_LoadSpritePal("sprite/personaje", 0);

	NF_LoadSpriteGfx("sprite/bola", 1, 32, 32);			// Bola azul
	NF_LoadSpritePal("sprite/bola", 1);


	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "capa_3");
	NF_CreateTiledBg(1, 2, "capa_2");


	// Transfiere a la VRAM los sprites necesarios
	NF_VramSpriteGfx(1, 0, 0, true);	// Bola (mantiene los frames en RAM)
	NF_VramSpritePal(1, 0, 0);

	NF_VramSpriteGfx(0, 1, 0, true);	// Personaje (mantiene los frames en RAM) Pantalla inferior
	NF_VramSpritePal(0, 1, 0);


	// Variables generales y inicializacion del random
	u16 n = 0;
	srand(time(NULL));

	// Crea el sprite del personaje en pantalla
	s16 pj_x = 0;
	s16 pj_y = 127;
	u8 pj_frame = 0;
	u8 pj_anim = 0;
	s8 pj_speed = 1;
	NF_CreateSprite(1, 0, 0, 0, pj_x, pj_y);

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
		NF_CreateSprite(0, n, 0, 0, bola_x[n], bola_y[n]);
	}

	// Variables para el ciclo de colores
	u8 r, g, b;
	s8 red, green, blue;
	s8 speed = 0;

	// Bucle (repite para siempre)
	while(1) {

		// Ciclo de colores
		speed ++;
		if (speed > 2) {
			speed = 0;

			// Bolas
			for (n = 1; n < 256; n ++) {
				// Obten el valor actual del color en la paleta
				NF_SpriteGetPalColor(0, 0, n, &r, &g, &b);
				// Pasa los valores del color a las variables de modificacion
				red = r;
				green = g;
				blue = b;
				// Modifica los valores
				red --;
				if (red < 0) red = 31;
				green ++;
				if (green > 31) green = 0;
				blue ++;
				if (blue > 31) blue = 0;
				// Actualiza el color en la paleta en RAM
				NF_SpriteEditPalColor(0, 0, n, red, green, blue);
			}
			// Actualiza la paleta en VRAM
			NF_SpriteUpdatePalette(0, 0);

			// Personaje
			for (n = 1; n < 256; n ++) {
				// Obten el valor actual del color en la paleta
				NF_SpriteGetPalColor(1, 0, n, &r, &g, &b);
				// Pasa los valores del color a las variables de modificacion
				red = r;
				green = g;
				blue = b;
				// Modifica los valores
				red --;
				if (red < 0) red = 31;
				//green ++;
				//if (green > 31) green = 0;
				//blue ++;
				//if (blue > 31) blue = 0;
				// Actualiza el color en la paleta en RAM
				NF_SpriteEditPalColor(1, 0, n, red, green, blue);
			}
			// Actualiza la paleta en VRAM
			NF_SpriteUpdatePalette(1, 0);
		}


		// Mueve el personaje
		pj_x += pj_speed;
		if ((pj_x < 0) || (pj_x > 191)) {
			pj_speed *= -1;
			if (pj_speed > 0) {
				NF_HflipSprite(1, 0, false);
			} else {
				NF_HflipSprite(1, 0, true);
			}
		}
		NF_MoveSprite(1, 0, pj_x, pj_y);

		// Animacion del personaje
		pj_anim ++;
		if (pj_anim > 5) {
			pj_anim = 0;
			pj_frame ++;
			if (pj_frame > 11) pj_frame = 0;
			NF_SpriteFrame(1, 0, pj_frame);
		}

		// Mueve las bolas
		for (n = 0; n < 32; n ++) {
			bola_x[n] += bola_spx[n];
			if ((bola_x[n] < 0) || (bola_x[n] > 223)) bola_spx[n] *= -1;
			bola_y[n] += bola_spy[n];
			if ((bola_y[n] < 0) || (bola_y[n] > 159)) bola_spy[n] *= -1;
			NF_MoveSprite(0, n, bola_x[n], bola_y[n]);
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
