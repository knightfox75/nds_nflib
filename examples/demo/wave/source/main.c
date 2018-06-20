/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de efecto onda (simple)

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


// Defines especiales del ejemplo
#define VLINE *(vu16*)0x04000006		// Obtiene via registro la linea actual del dibujado


// Variables
u16 mapa[32][24];	// Almacena el mapa
s16 bgx[192];		// Posicion X de cada linea
s8 i[192];			// Control de movimiento de cada linea




// Define la funcion a ejecutar en cada HBLANK
void _WaveEffect(void);





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

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "nfl");

	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "nfl");

	// Invierte todos los tiles (V) del mapa de la pantalla inferior y copia los tiles al buffer temporal
	s16 x = 0;
	s16 y = 0;
	for (y = 0; y < 24; y ++) {
		for (x = 0; x < 32; x ++) {
			NF_SetTileVflip(1, 3, x, y);
			mapa[x][y] = NF_GetTileOfMap(1, 3, x, y);
		}
	}
	// Ahora coloca los tiles en orden inverso para compensar la imagen
	for (y = 0; y < 24; y ++) {
		for (x = 0; x < 32; x ++) {
			NF_SetTileOfMap(1, 3, x, (23 - y), mapa[x][y]);
		}
	}
	// Y para finalizar, actualiza el mapa en la VRAM
	NF_UpdateVramMap(1, 3);

	// Inicializa el efecto wave
	s8 inc = 1;
	x = 0;
	for (y = 0; y < 192; y ++) {
		x += inc;
		bgx[y] = x;
		if ((x < 1) || (x > 63)) inc *= -1;
		i[y] = inc;
	}

	// Definimos que funcion se ejecutara al final de cada linea de dibujado (HBLANK)
	irqSet(IRQ_HBLANK, _WaveEffect);

	// Para finalizar, habilitamos la interrupcion
	irqEnable(IRQ_HBLANK);

	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();		// Espera al sincronismo vertical

	}

	return 0; 

}




// Efecto WAVE (Se ejecuta al final de cada linea de dibujado)
void _WaveEffect(void) {

	if (VLINE < 192) {	// Si la linea esta entre la 0 - 191, ejecuta el efecto
		bgx[VLINE] += i[VLINE];
		if ((bgx[VLINE] < 1) || (bgx[VLINE] > 63)) i[VLINE] *= -1;
		NF_ScrollBg(1, 3, ((bgx[VLINE] >> 3) - 4), 0);
	}

}
