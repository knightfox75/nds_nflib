/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de mapas de colisiones por pixels
	en superficies inclinadas

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
	NF_LoadTiledBg("bg/pdemo_bg", "bg3", 256, 256);		// Carga el fondo para la capa 3, pantalla superior

	// Carga los archivos de sprites
	NF_LoadSpriteGfx("sprite/whiteball", 0, 16, 16);	// Pelota
	NF_LoadSpritePal("sprite/whitepal", 0);

	// Carga el fondo de colisiones
	NF_LoadColisionBg("maps/pdemo_colmap", 0, 256, 256);

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "bg3");

	// Transfiere a la VRAM los sprites necesarios
	NF_VramSpriteGfx(0, 0, 0, true);	// Puntero
	NF_VramSpritePal(0, 0, 0);

	// Variables de uso genereal
	u8 b = 0;
	u8 n = 0;

	// Crea el Sprite del puntero en la pantalla inferior
	for (b = 0; b < 3; b ++) {
		NF_CreateSprite(0, b, 0, 0, -16, -16);	// Crea el puntero en la pantalla inferior
		NF_SpriteLayer(0, b, 3);				// Y la capa sobre la que se dibujara
	}

	// Variables para el control de movimiento
	s16 x[3];
	s16 y[3];
	x[0] = 32;
	y[0] = -16;
	x[1] = 228;
	y[1] = 32;
	x[2] = 10;
	y[2] = 100;


	// Variables de control de colisiones, define todos los puntos de colision del sprite
	s16 py[16];
	py[0] = 11;
	py[1] = 13;
	py[2] = 14;
	py[3] = 15;
	py[4] = 15;
	py[5] = 16;
	py[6] = 16;
	py[7] = 16;
	py[8] = 16;
	py[9] = 16;
	py[10] = 16;
	py[11] = 15;
	py[12] = 15;
	py[13] = 14;
	py[14] = 13;
	py[15] = 11;

	// Control de movimiento
	bool down = false;
	bool left = false;
	bool right = false;

	// Bucle (repite para siempre)
	while(1) {

		// Borra la pantalal de texto
		consoleClear();

		// Bola a bola
		for (b = 0; b < 3; b ++) {

			// Control de colisiones, caida
			down = true;	// Flag de descenso arriba
			// Busca pixel por pixel, si hay colisiones (pixel azul, nº4)
			for (n = 0; n < 16; n ++) {
				if (NF_GetPoint(0, (x[b] + n), (y[b] + py[n])) == 4) down = false;
			}

			// Control de colisiones, decide derecha o izquierda
			right = true;	// Flag de movimiento lateral arriba
			left = true;
			// Caida a izquierda
			if (NF_GetPoint(0, (x[b] - 1), (y[b] + 16)) == 4) left = false;
			// Caida a derecha
			if (NF_GetPoint(0, (x[b] + 16), (y[b] + 16)) == 4) right = false;
			// Si hay caida libre, no te muevas en horizontal
			if (left && right) {
				right = false;
				left = false;
			}

			// Si es necesario, caida libre
			if (down) y[b] ++;
			// Muevete a la derecha
			if (right) x[b] ++;
			// Muevete a la izquierda
			if (left) x[b] --;

			// Recoloca la pelota si sale de los limites de pantalla
			if (y[b] > 192) {
				x[b] = 32;
				y[b] = -16;
			}

			// Posicion del Sprite
			NF_MoveSprite(0, b, x[b], y[b]);

			// Imprime la posicion de la pelota
			iprintf("x:%03d  y:%03d\n", x[b], y[b]);

		}

		NF_SpriteOamSet(0);				// Actualiza el Array del OAM

		swiWaitForVBlank();				// Espera al sincronismo vertical

		oamUpdate(&oamMain);			// Actualiza a VRAM el OAM Secundario
	
	}

	return 0; 

}
