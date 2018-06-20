/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de 3D Sprites animados

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

#define SPRITEMAXNUM 255

int main(int argc, char **argv) {

	// Inicializa el random
	srand(time(NULL));

	// Pantalla de espera inicializando NitroFS
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);
	consoleDemoInit();
	iprintf("\n NitroFS init. Please wait.\n\n");
	iprintf(" Iniciando NitroFS,\n por favor, espere.\n\n");
	swiWaitForVBlank();

	// Define el ROOT e inicializa el sistema de archivos
	NF_SetRootFolder("NITROFS");	// Define la carpeta ROOT para usar NITROFS

	// Inicializa el motor 3D
	NF_Set3D(0, 0);				// Modo 3D_0 en la pantalla superior

	// Inicializa el motor 2D
	NF_Set2D(1, 0);				// Modo 2D_0 en la pantalla inferior

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior
	NF_InitTiledBgSys(1);		// Inicializa los fondos Tileados para la pantalla inferior

	// Inicializa los buffers de los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas

	// Inicializa el sistema de Sprites en 3D y OpenGL
	NF_Init3dSpriteSys();

	// Carga los fondos tileados
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);
	NF_LoadTiledBg("bg/bg3", "bg3", 256, 256);

	// Carga los graficos de los Sprites
	u8 n = 0;
	for (n = 0; n < SPRITEMAXNUM; n ++) {
		NF_LoadSpriteGfx("sprite/numbers", n, 16, 16);
	}
	NF_LoadSpritePal("sprite/numbers", 0);


	// Manda los graficos y paletas a la VRAM
	for (n = 0; n < SPRITEMAXNUM; n ++) {
		NF_Vram3dSpriteGfx(n, n, true);
	}
	NF_Vram3dSpritePal(0, 0);

	// Crea los fondos en ambas pantallas
	NF_CreateTiledBg(0, 3, "bg3");
	NF_CreateTiledBg(1, 3, "nfl");


	// Variables
	s16 x[SPRITEMAXNUM];
	s16 y[SPRITEMAXNUM];
	s8 ix[SPRITEMAXNUM];
	s8 iy[SPRITEMAXNUM];
	u16 timer[SPRITEMAXNUM];
	u8 frame[SPRITEMAXNUM];

	// Inicializa las variables
	for (n = 0; n < SPRITEMAXNUM; n ++) {
		x[n] = (((int)(rand() % 239)));
		y[n] = (((int)(rand() % 175)));
		timer[n] = (((int)(rand() % 20)));
		frame[n] = (((int)(rand() % 10)));
		if ((rand() % 100) > 50) {
			ix[n] = 1;
		} else {
			ix[n] = -1;
		}
		if ((rand() % 100) > 50) {
			iy[n] = 1;
		} else {
			iy[n] = -1;
		}
		// Crea los Sprites 3D
		NF_Create3dSprite(n, n, 0, x[n], y[n]);
		NF_Set3dSpriteFrame(n, frame[n]);
	}

	// Y ordenalos en la cola segun su ID (El mas bajo tiene prioridad)
	NF_Sort3dSprites();

	// Bucle (repite para siempre)
	while(1) {

		// Mueve todos los Sprites
		for (n = 0; n < SPRITEMAXNUM; n ++) {
			x[n] += ix[n];
			if ((x[n] < 0) || (x[n] > (255 - NF_3DSPRITE[n].width))) ix[n] = -ix[n];
			y[n] += iy[n];
			if ((y[n] < 0) || (y[n] > (191 - NF_3DSPRITE[n].height))) iy[n] = -iy[n];
			timer[n] ++;
			if (timer[n] > 19) {
				timer[n] = 0;
				frame[n] ++;
				if (frame[n] > 9) frame[n] = 0;
			}
			NF_Move3dSprite(n, x[n], y[n]);
			NF_Set3dSpriteFrame(n, frame[n]);
		}

		// Dibuja los 3D Sprites
		NF_Draw3dSprites();

		// Actualiza la escena 3D, si no lo haces, no se mostrara en pantalla
		glFlush(0);

		// Espera al sincronismo vertical
		swiWaitForVBlank();

		// Actualiza las texturas de los 3D Sprites animados con KEEPFRAMES == TRUE
		NF_Update3dSpritesGfx();


	}

	return 0;

}
