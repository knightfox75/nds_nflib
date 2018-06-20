/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de reflejo del agua (con scroll)

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


// Variables del efecto wave
u16 mapa[32][24];	// Almacena el mapa
s16 bgx[192];		// Posicion X de cada linea
s8 i[192];			// Control de movimiento de cada linea

// Variables para el control de fondo
s16 bg_x[4];


// Define la funcion a ejecutar para el HBLANK
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
	NF_LoadTiledBg("bg/layer0", "layer0", 512, 256);	// Carga el fondo para la pantalla inferior
	NF_LoadTiledBg("bg/layer1", "layer1", 512, 256);	// Carga el fondo para la pantalla inferior
	NF_LoadTiledBg("bg/layer2", "layer2", 512, 256);	// Carga el fondo para la pantalla inferior
	NF_LoadTiledBg("bg/layer3", "layer3", 512, 256);	// Carga el fondo para la pantalla inferior
	NF_LoadTiledBg("bg/nfl", "nfl", 256, 256);			// Carga el fondo para la pantalla inferior

	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "layer3");
	NF_CreateTiledBg(0, 2, "layer2");
	NF_CreateTiledBg(0, 1, "layer1");
	NF_CreateTiledBg(0, 0, "layer0");

	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "nfl");

	// Inicializa el efecto wave
	s16 y = 0;
	s8 inc = 1;
	for (y = 138; y < 192; y ++) {
		// Calculos para la capa 1
		if ((y >= 138) && (y <= 159)) {
			bgx[y] = 32;
			inc *= -1;	
			i[y] = inc;
		}
		// Calculos para la capa 0
		if ((y >= 160) && (y <= 191)) {
			bgx[y] = ((y - 156) >> 1);
			i[y] = inc;
			inc *= -1;
		}
	}

	// Y definimos que funcion se ejecutara al final de cada linea de dibujado (HBLANK)
	irqSet(IRQ_HBLANK, _WaveEffect);

	// Para finalizar, habilitamos la interrupcion
	irqEnable(IRQ_HBLANK);

	// Lectura del keypad
	u16 keys = 0;

	// Bucle (repite para siempre)
	while(1) {

		// Lee el keypad
		scanKeys();
		keys = keysHeld();

		// Mueve el fondo
		if ((keys & KEY_LEFT) && (bg_x[0] > 8)) bg_x[0] --;
		if ((keys & KEY_RIGHT) && (bg_x[0] < 247)) bg_x[0] ++;

		// Calcula el parallax
		bg_x[1] = (int)(bg_x[0] / 1.5);
		bg_x[2] = (int)(bg_x[1] / 1.5);
		bg_x[3] = (int)(bg_x[2] / 1.5);

		swiWaitForVBlank();		// Espera al sincronismo vertical

		// Posiciona cada fondo donde le corresponde
		NF_ScrollBg(0, 3, bg_x[3], 0);
		NF_ScrollBg(0, 2, bg_x[2], 0);
		NF_ScrollBg(0, 1, bg_x[1], 0);
		NF_ScrollBg(0, 0, bg_x[0], 0);

	}

	return 0; 

}




void _WaveEffect(void) {

	if ((VLINE >= 138) && (VLINE <= 159)) {	// Si la linea esta entre la 138 - 159, ejecuta el efecto para la capa 1
		bgx[VLINE] += i[VLINE];
		if ((bgx[VLINE] < 1) || (bgx[VLINE] > 63)) i[VLINE] *= -1;
		NF_ScrollBg(0, 1, bg_x[1] + ((bgx[VLINE] >> 3) - 4), 0);
	}
	if ((VLINE >= 160) && (VLINE <= 191)) {	// Si la linea esta entre la 160 - 191, ejecuta el efecto para la capa 0
		bgx[VLINE] += i[VLINE];
		if ((bgx[VLINE] < 1) || (bgx[VLINE] > (VLINE - 156))) i[VLINE] *= -1;
		NF_ScrollBg(0, 0, (bg_x[0] + (((VLINE - 156) >> 1) - bgx[VLINE])), 0);
	}

}
