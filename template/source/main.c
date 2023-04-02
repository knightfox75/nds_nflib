// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011

/*
-------------------------------------------------

	NightFox's Lib Template

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por NightFox
	http://www.nightfoxandco.com
	Inicio 10 de Octubre del 2009

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

	consoleDemoInit();		// Inicializa la consola de texto
	consoleClear();			// Borra la pantalla
	setBrightness(3, 0);	// Restaura el brillo

	iprintf("\n Hello World!");

	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();		// Espera al sincronismo vertical

	}

	return 0;

}
