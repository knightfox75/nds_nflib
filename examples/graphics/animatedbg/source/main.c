/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de fondos animados (Tile animation)

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


// Algunos defines interesantes
#define SPEED 2				// Velocidad del scroll
#define MAP_X 16			// Ancho del mapa (bloques de animacion de 4x4 tiles)
#define MAP_Y 16			// Alto del mapa (bloques de animacion de 4x4 tiles)
#define WATER_FRAMES 50		// Frames totales de la animacion
#define WATER_DELAY 10		// Velocidad de la animacion (espera entre frames)


// Declara funciones
void InitAnimatedBg(void);	// Inicializa la animacion
void AnimateWater(void);	// Animacion del fondo


// Declara las variables globales
u8 WATER_SPEED;
u8 WATER_FRAME[MAP_X][MAP_Y];		// 512 pixeles, 64 tiles, cada frame son 4x4 tiles






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

	// Carga los archivos de fondo desde la FAT / NitroFS
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);						// Carga el fondo para la pantalla superior
	NF_LoadTilesForBg("bg/water", "water", 512, 512, 0, 175);		// Carga el tileset para el agua

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");
	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "water");

	// Inicializa la animacion del fondo
	InitAnimatedBg();


	// Variables
	s16 bg_x = 0;
	s16 bg_y = 0;

	u16 keys = 0;


	// Bucle (repite para siempre)
	while(1) {

		// Lee el keypad
		scanKeys();
		keys = keysHeld();		// Teclas presionadas

		// Si pulsas izquierda
		if (keys & KEY_LEFT) {
			bg_x -= SPEED;
			if (bg_x < 0) bg_x = 0;
		}

		// Si pulsas derecha
		if (keys & KEY_RIGHT) {
			bg_x += SPEED;
			if (bg_x > 255) bg_x = 255;		// Limite fisico de la capa
		}

		// Si pulsas arriba
		if (keys & KEY_UP) {
			bg_y -= SPEED;
			if (bg_y < 0) bg_y = 0;
		}

		// Si pulsas abajo
		if (keys & KEY_DOWN) {
			bg_y += SPEED;
			if (bg_y > 255) bg_y = 255;		// Limite fisico de la capa
		}

		// Anima el fondo
		AnimateWater();	

		swiWaitForVBlank();		// Espera al sincronismo vertical

		// Actualiza la posicion de los fondos
		NF_ScrollBg(1, 3, bg_x, bg_y);
	
	}

	return 0; 

}




// Funcion para inicializar el fondo animado
void InitAnimatedBg(void) {

	// Variables locales
	u8 x = 0;
	u8 y = 0;
	u8 tile_x = 0;
	u8 tile_y = 0;
	u8 start_x = 0;
	u8 end_x = 0;
	u8 start_y = 0;
	u8 end_y = 0;
	u8 n = 0;
	u8 frame = 0;

	// Inicializa la animacion del agua
	srand(time(NULL));
	for (y = 0; y < MAP_Y; y ++) {
		for (x = 0; x < MAP_X; x ++) {
			WATER_FRAME[x][y] = (rand() % (WATER_FRAMES + 1));
		}
	}

	// Genera el mapa de el fondo marino
	for (y = 0; y < MAP_Y; y ++) {
		for (x = 0; x < MAP_X; x ++) {
			// Calcula el area a rellenar
			start_x = (x << 2);
			end_x = start_x + 4;
			start_y = (y << 2);
			end_y = start_y +4;
			// Desplazamiento
			n = 0;
			// Rellena la cuadricula de 4x4 tiles
			for (tile_y = start_y; tile_y < end_y; tile_y ++) {
				for (tile_x = start_x; tile_x < end_x; tile_x ++) {
					frame = WATER_FRAME[x][y];
					if (frame > 10) frame = 10;		// La animacion tiene 11 frames, por encima de estos, muestra siempre el mismo (sensacion de "reposo")
					NF_SetTileOfMap(1, 3, tile_x, tile_y, ((frame << 4) + n));
					n ++;
				}
			}				
		}
	}

	// Actualiza el mapa en VRAM
	NF_UpdateVramMap(1, 3);

}



// Funcion para animar el fondo mediante tiles
void AnimateWater(void) {

	// Calcula el movimiento
	WATER_SPEED ++;

	// Si debes cambiar de frame...
	if (WATER_SPEED >= WATER_DELAY) {

		// Reset del contador de espera
		WATER_SPEED = 0;

		// Variables temporales
		u8 x = 0;
		u8 y = 0;
		u8 tile_x = 0;
		u8 tile_y = 0;
		u8 start_x = 0;
		u8 end_x = 0;
		u8 start_y = 0;
		u8 end_y = 0;
		u8 n = 0;

		// Cambia los tiles necesarios para realizar la animacion
		for (y = 0; y < MAP_Y; y ++) {
			for (x = 0; x < MAP_X; x ++) {
				// Calcula el area a rellenar en este ciclo
				start_x = (x << 2);
				end_x = start_x + 4;
				start_y = (y << 2);
				end_y = start_y + 4;
				// Y el frame a ejecutar
				WATER_FRAME[x][y] ++;
				if (WATER_FRAME[x][y] > WATER_FRAMES) WATER_FRAME[x][y] = 0;
				if (WATER_FRAME[x][y] <= 10) {
					// Desplazamiento
					n = 0;
					// Rellena la cuadricula de 4x4 tiles
					for (tile_y = start_y; tile_y < end_y; tile_y ++) {
						for (tile_x = start_x; tile_x < end_x; tile_x ++) {
							NF_SetTileOfMap(1, 3, tile_x, tile_y, ((WATER_FRAME[x][y] << 4) + n));
							n ++;
						}
					}
				}
			}
		}

		// Actualiza el mapa en la VRAM
		NF_UpdateVramMap(1, 3);
				
	}

}
