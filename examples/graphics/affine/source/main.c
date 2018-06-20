/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de fondos Affine

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por NightFox
	http://www.nightfoxandco.com/
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

// Funcion encargada del sombreado
void Shading(void);



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
	NF_Set2D(0, 2);				// Modo 2D_2 en la pantalla superior
	NF_Set2D(1, 2);				// Modo 2D_2 en la pantalla inferior

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitAffineBgSys(0);		// Inicializa los fondos affine para la pantalla superior
	NF_InitAffineBgSys(1);		// Iniciliaza los fondos affine para la pantalla inferior

	// Carga los archivos de fondo desde NitroFS a la RAM
	NF_LoadAffineBg("bg/waves512", "waves", 512, 512);		// Carga el fondo para la capa 3, pantalla superior
	NF_LoadAffineBg("bg/navygrid", "grid", 256, 256);		// Carga el fondo para la capa 2, pantalla superior
	NF_LoadAffineBg("bg/flag512", "flag", 512, 512);		// Carga el fondo para la capa 3, pantalla inferior

	// Crea los fondos de la pantalla Superior
	NF_CreateAffineBg(0, 3, "waves", 1);
	NF_CreateAffineBg(0, 2, "grid", 0);

	// Crea los fondos de la pantalla Inferior
	NF_CreateAffineBg(1, 3, "flag", 1);

	// Definimos que funcion se ejecutara al final de cada linea de dibujado (HBLANK)
	irqSet(IRQ_HBLANK, Shading);

	// Para finalizar, habilitamos la interrupcion
	irqEnable(IRQ_HBLANK);

	// Variable para la lectura del keypad
	u16 keys = 0;

	// Variables para el control de movimiento
	s32 x = 0;
	s32 y = 0;
	s32 angle = 0;
	s32 zoom = 256;
	u8 n = 0;

	// Efectos especiales de sombreado
	REG_BLDCNT = BLEND_FADE_WHITE | BLEND_SRC_BG3;		// Fade a blanco
	REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3;	// Fade a negro


	// Bucle (repite para siempre)
	while(1) {

		// Lee el teclado
		scanKeys();
		keys = keysHeld();

		// Calcula el desplazamiento del centro
		if (keys & KEY_UP) y -= 2;
		if (y < 0) y = 0;
		if (keys & KEY_DOWN) y += 2;
		if (y > 512) y = 512;
		if (keys & KEY_LEFT) x -= 2;
		if (x < 0) x = 0;
		if (keys & KEY_RIGHT) x += 2;
		if (x > 512) x = 512;

		// Calcula el Zoom
		if (keys & KEY_A) zoom -= 2;
		if (zoom < 0) zoom = 0;
		if (keys & KEY_B) zoom += 2;
		if (zoom > 512) zoom = 512;

		// Calcula el angulo
		if (keys & KEY_X) angle -= 2;
		if (angle < 0) angle += 2048;
		if (keys & KEY_Y) angle += 2;
		if (angle > 2048) angle -= 2048;

		swiWaitForVBlank();				// Espera al sincronismo vertical

		// Modifica los parametros del fondo Affine
		for (n = 0; n < 2; n ++) {
			// Zoom
			NF_AffineBgTransform(n, 3, zoom, zoom, 0, 0);
			// Posicion del centro
			NF_AffineBgCenter(n, 3, x, y);
			// Rotacion
			NF_AffineBgMove(n, 3, 0, 0, angle);
		}

	}

	return 0; 

}



// Funcion de Shading
void Shading(void) {

	// Si la linea de dibujado esta en pantalla
	if (VLINE < 192) {
		// Calcula el valor del sombreado segun la linea
		s16 fade = (0x0F - (((VLINE + 1) * 16) / 192));
		// Sombreado "Blanco"
		REG_BLDY  = (fade >> 1);
		// Sombreado "Negro"
		REG_BLDY_SUB  = fade;
	}

}
