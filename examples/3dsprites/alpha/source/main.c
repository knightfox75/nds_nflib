/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de 3D Sprites
	Transparencias (Alpha Blending)

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

	// Pantalla de espera inicializando NitroFS
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);
	consoleDemoInit();
	iprintf("\n NitroFS init. Please wait.\n\n");
	iprintf(" Iniciando NitroFS,\n por favor, espere.\n\n");
	swiWaitForVBlank();

	// Define el ROOT e inicializa el sistema de archivos
	NF_SetRootFolder("NITROFS");	// Define la carpeta ROOT para usar NITROFS
	consoleClear();

	// Inicializa el motor 3D
	NF_Set3D(1, 0);				// Modo 3D_0 en la pantalla superior

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior

	// Inicializa los buffers de los Sprites
	NF_InitSpriteBuffers();		// Inicializa los buffers para almacenar sprites y paletas

	// Inicializa el sistema de Sprites en 3D y OpenGL
	NF_Init3dSpriteSys();

	// Carga los fondos tileados
	NF_LoadTiledBg("bg/nature", "bg3", 256, 256);

	// Carga los graficos de los Sprites
	NF_LoadSpriteGfx("sprite/blueball", 0, 64, 64);
	NF_LoadSpritePal("sprite/blueball", 0);

	// Manda los graficos y paletas a la VRAM
	NF_Vram3dSpriteGfx(0, 0, true);
	NF_Vram3dSpritePal(0, 0);

	// Crea los fondos en ambas pantallas
	NF_CreateTiledBg(0, 3, "bg3");

	// Habilita el ALPHA de los Sprites 3D sobre fondos 2D
	REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG0 | (BLEND_DST_BG1 + BLEND_DST_BG2 + BLEND_DST_BG3);

	// Variables
	u16 n = 0;
	s16 x[MAXSPRITES];
	s16 y[MAXSPRITES];
	s16 ix = 4;
	s16 iy = 4;

	// Inicializa las variables
	for (n = 0; n < MAXSPRITES; n ++) {
		x[n] = (128 - 32);
		y[n] = (96 - 32);
		// Crea los Sprites 3D
		NF_Create3dSprite(n, 0, 0, x[n], y[n]);
	}

	// Y ordenalos en la cola segun su ID (El mas bajo tiene prioridad)
	NF_Sort3dSprites();

	// Variable para la lectura del teclado
	u16 held = 0;

	// Variables para la lectura del touchpad
	touchPosition touchscreen;

	// Bucle (repite para siempre)
	while(1) {

		// Lee el teclado
		scanKeys();
		held = keysHeld();

		// Lee el TOUCHPAD via Libnds
		touchRead(&touchscreen);

		// Mueve todos los Sprites
		for (n = (MAXSPRITES - 1); n > 0; n --) {
			// Calcula la estela del movimiento
			x[n] = x[(n - 1)];
			y[n] = y[(n - 1)];
			// Calcula el blending
			NF_Blend3dSprite(n, (n + 1), (31 - (n << 2)));
			// Mueve el sprite
			NF_Move3dSprite(n, x[n], y[n]);
		}

		// Mueve el Sprite principal
		if (held & KEY_TOUCH) {
			x[0] = (touchscreen.px - 32);
			y[0] = (touchscreen.py - 32);
			if (x[0] < 8) x[0] = 8;
			if (x[0] > 183) x[0] = 183;
			if (y[0] < 8) y[0] = 8;
			if (y[0] > 119) y[0] = 119;
		} else {
			x[0] += ix;
			if ((x[0] < 8) || (x[0] > 183)) ix = -ix;
			y[0] += iy;
			if ((y[0] < 8) || (y[0] > 119)) iy = -iy;
		}
		NF_Move3dSprite(0, x[0], y[0]);

		// Dibuja los 3D Sprites
		NF_Draw3dSprites();

		// Actualiza la escena 3D, si no lo haces, no se mostrara en pantalla
		glFlush(0);

		// Espera al sincronismo vertical
		swiWaitForVBlank();

	}

	return 0;

}
