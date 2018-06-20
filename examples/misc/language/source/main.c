/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo de obtencion del idioma del usuario

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

	// Inicializa las pantallas
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);	
	consoleDemoInit();
	swiWaitForVBlank();

	// Imprime en pantalla el idioma del usuario
	switch (NF_GetLanguage()) {
		case 0 : // Japanese
			iprintf("Japanese");
			break;
		case 1 : // English
			iprintf("English");
			break;
		case 2 : // French
			iprintf("French");
			break;
		case 3 : // German
			iprintf("German");
			break;
		case 4 : // Italian
			iprintf("Italian");
			break;
		case 5 : // Spanish
			iprintf("Spanish");
			break;
		case 6 : // Chinese
			iprintf("Chinese");
			break;
		default:
			iprintf("Error");
			break;
	}

	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();			// Espera al sincronismo vertical
	
	}

	return 0; 

}
