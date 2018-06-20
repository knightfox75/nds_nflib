/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de 3D Sprites
	Intercambio de prioridades

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


#define MAXSPRITES 32


/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

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

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior

	// Inicializa los buffers de los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas

	// Inicializa el sistema de Sprites en 3D y OpenGL
	NF_Init3dSpriteSys();

	// Carga los fondos tileados
	NF_LoadTiledBg("bg/bg3", "bg3", 256, 256);

	// Carga los graficos de los Sprites
	NF_LoadSpriteGfx("sprite/blueball", 0, 64, 64);
	NF_LoadSpritePal("sprite/blueball", 0);
	NF_LoadSpriteGfx("sprite/redcar", 1, 128, 64);
	NF_LoadSpritePal("sprite/redcar", 1);

	// Manda los graficos y paletas a la VRAM
	NF_Vram3dSpriteGfx(0, 0, true);
	NF_Vram3dSpritePal(0, 0);
	NF_Vram3dSpriteGfx(1, 1, true);
	NF_Vram3dSpritePal(1, 1);


	// Crea los fondos en ambas pantallas
	NF_CreateTiledBg(0, 3, "bg3");


	// Variables
	u16 n = 0;
	u16 r = 0;
	s16 x[MAXSPRITES];
	s16 y[MAXSPRITES];
	s16 ix[MAXSPRITES];
	s16 iy[MAXSPRITES];

	// Inicializa las variables
	for (n = 0; n < MAXSPRITES; n ++) {
		if ((n == 0) || (n == (MAXSPRITES - 1))) {
			r = 1;
		} else {
			r = 0;
		}
		x[n] = (((int)(rand() % 128)) + 1);
		y[n] = (((int)(rand() % 112)) + 1);
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
		NF_Create3dSprite(n, r, r, x[n], y[n]);
	}

	// Y ordenalos en la cola segun su ID (El mas bajo tiene prioridad)
	NF_Sort3dSprites();

	// Variable para la lectura del teclado
	u16 keys = 0;

	// Bucle (repite para siempre)
	while(1) {

		// Lee el teclado
		scanKeys();
		keys = keysDown();

		// Mueve todos los Sprites
		for (n = 0; n < MAXSPRITES; n ++) {
			x[n] += ix[n];
			if ((x[n] < 1) || (x[n] > (255 - NF_3DSPRITE[n].width))) ix[n] = -ix[n];
			y[n] += iy[n];
			if ((y[n] < 1) || (y[n] > (191 - NF_3DSPRITE[n].height))) iy[n] = -iy[n];
			NF_Move3dSprite(n, x[n], y[n]);
		}

		// Muestra u oculta los sprites pares
		if (keys & KEY_A) {
			NF_Swap3dSpritePriority(0, (MAXSPRITES - 1));
		}
		if (keys & KEY_B) {
			NF_Swap3dSpritePriority((MAXSPRITES - 1), 0);
		}

		// Dibuja los 3D Sprites
		NF_Draw3dSprites();

		// Actualiza la escena 3D, si no lo haces, no se mostrara en pantalla
		glFlush(0);

		// Debug
		consoleClear();
		iprintf("\nSprite Id%02d is on %02d priority\n", 0, NF_3DSPRITE[0].prio);
		iprintf("Sprite Id%02d is on %02d priority\n\n", (MAXSPRITES - 1), NF_3DSPRITE[(MAXSPRITES - 1)].prio);
		iprintf("A or B - Swap 0 & %d", (MAXSPRITES - 1));

		// Espera al sincronismo vertical
		swiWaitForVBlank();
	
	}

	return 0; 

}
