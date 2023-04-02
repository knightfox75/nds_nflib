// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2009-2011

/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de obtencion del idioma del usuario

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

	// Inicializa las pantallas
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);	
	consoleDemoInit();
	swiWaitForVBlank();

	// Imprime en pantalla el idioma del usuario
	switch (NF_GetLanguage()) {
		case 0 : // Japanese
			printf("Japanese");
			break;
		case 1 : // English
			printf("English");
			break;
		case 2 : // French
			printf("French");
			break;
		case 3 : // German
			printf("German");
			break;
		case 4 : // Italian
			printf("Italian");
			break;
		case 5 : // Spanish
			printf("Spanish");
			break;
		case 6 : // Chinese
			printf("Chinese");
			break;
		default:
			printf("Error");
			break;
	}

	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();			// Espera al sincronismo vertical
	
	}

	return 0; 

}
