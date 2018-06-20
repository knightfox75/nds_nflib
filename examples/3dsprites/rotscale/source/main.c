/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de 3D Sprites
	Rotacion y escalado

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


#define MAXSPRITES 8


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
	s16 rx[MAXSPRITES];
	s16 ry[MAXSPRITES];
	s16 rz[MAXSPRITES];
	s16 scale[MAXSPRITES];

	// Inicializa las variables
	for (n = 0; n < MAXSPRITES; n ++) {
		if ((n % 2) == 0) {
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
		rx[n] = 0;
		ry[n] = 0;
		rz[n] = 0;
		scale[n] = 64;
		// Crea los Sprites 3D
		NF_Create3dSprite(n, r, r, x[n], y[n]);
	}

	// Y ordenalos en la cola segun su ID (El mas bajo tiene prioridad)
	NF_Sort3dSprites();

	// Variable para la lectura del teclado
	u16 press = 0;
	u16 held = 0;
	s16 id = 0;

	// Bucle (repite para siempre)
	while(1) {

		// Lee el teclado
		scanKeys();
		press = keysDown();
		held = keysHeld();

		// Mueve todos los Sprites
		for (n = 0; n < MAXSPRITES; n ++) {
			x[n] += ix[n];
			if ((x[n] < 1) || (x[n] > (255 - NF_3DSPRITE[n].width))) ix[n] = -ix[n];
			y[n] += iy[n];
			if ((y[n] < 1) || (y[n] > (191 - NF_3DSPRITE[n].height))) iy[n] = -iy[n];
			NF_Move3dSprite(n, x[n], y[n]);
		}

		// Selecciona el Sprite con A y B
		if (press & KEY_A) {
			id ++;
			if (id > (MAXSPRITES - 1)) id = 0;
		}
		if (press & KEY_B) {
			id --;
			if (id < 0) id = (MAXSPRITES - 1);
		}

		// Rota el Z del Sprite ID
		if (held & KEY_RIGHT) {
			rz[id] += 2;
			if (rz[id] > 512) rz[id] -= 512;
		}
		if (held & KEY_LEFT) {
			rz[id] -= 2;
			if (rz[id] < 0) rz[id] += 512;
		}

		// Rota el Y del Sprite ID
		if (held & KEY_DOWN) {
			ry[id] += 2;
			if (ry[id] > 512) ry[id] -= 512;
		}
		if (held & KEY_UP) {
			ry[id] -= 2;
			if (ry[id] < 0) ry[id] += 512;
		}

		// Rota el X del Sprite ID
		if (held & KEY_X) {
			rx[id] += 2;
			if (rx[id] > 512) rx[id] -= 512;
		}
		if (held & KEY_Y) {
			rx[id] -= 2;
			if (rx[id] < 0) rx[id] += 512;
		}

		// Escala el sprite
		if (held & KEY_R) {
			scale[id] += 2;
			if (scale[id] > 512) scale[id] = 512;
		}
		if (held & KEY_L) {
			scale[id] -= 2;
			if (scale[id] < 0) scale[id] = 0;
		}

		// Aplica la rotacion
		NF_Rotate3dSprite(id, rx[id], ry[id], rz[id]);

		// Aplica el escalado
		NF_Scale3dSprite(id, scale[id], scale[id]);

		// Dibuja los 3D Sprites
		NF_Draw3dSprites();

		// Actualiza la escena 3D, si no lo haces, no se mostrara en pantalla
		glFlush(0);

		// Debug
		consoleClear();
		iprintf("A / B - Sprite select %d\n", id);
		iprintf("Rotate Z (LEFT/RIGHT) %d\n", rz[id]);
		iprintf("Rotate Y (UP/DOWN) %d\n", ry[id]);
		iprintf("Rotate X (X/Y) %d\n", rx[id]);
		iprintf("Scale (R/L) %d\n", scale[id]);

		// Espera al sincronismo vertical
		swiWaitForVBlank();
	
	}

	return 0; 

}
