/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico de BackBuffer en 16bits (modo BITMAP)
	Zoom x3 con filtro por interpolacion

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
#include <string.h>
#include <unistd.h>
#include <time.h>

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

	// Inicializa el motor 2D en modo BITMAP
	NF_Set2D(0, 5);				// Modo 2D_5 en ambas pantallas
	NF_Set2D(1, 5);

	// Inicializa los fondos en modo "BITMAP"
	NF_InitBitmapBgSys(0, 1);
	NF_InitBitmapBgSys(1, 1);

	// Inicializa los buffers para guardar fondos en formato BITMAP
	NF_Init16bitsBgBuffers();

	// Inicializa el BackBuffer de 16 bits
	// Usalo solo una vez antes de habilitarlo
	NF_Init16bitsBackBuffer(0);
	NF_Init16bitsBackBuffer(1);

	// Habilita el backbuffer en la pantalla superior
	NF_Enable16bitsBackBuffer(0);
	NF_Enable16bitsBackBuffer(1);

	// Carga el archivo BITMAP de imagen en formato RAW a la RAM
	NF_Load16bitsBg("bmp/img16_c", 0);

	// Tranfiere la imagen al BackBuffer de la pantalla inferior
	NF_Copy16bitsBuffer(1, 1, 0);

	// Variables RGB
	u8 r[5];
	u8 g[5];
	u8 b[5];
	u8 red = 0;
	u8 green = 0;
	u8 blue = 0;

	// Calculo del valor RGB
	u16 rgb = 0;

	// Ventana
	s16 x = 0;
	s16 y = 0;
	s16 composite = 0;
	s16 square_x = 0;
	s16 square_y = 0;
	s16 zoom_x = 0;
	s16 zoom_y = 0;

	// Variables del control del touchpad
	u16 keys = 0;
	touchPosition touchscreen;

	u32 loops = 0;

	while (1) {

		// Copia el original
		NF_Copy16bitsBuffer(1, 1, 0);

		// Lectura de posicion del stylus
		scanKeys();				// Lee el touchpad via Libnds
		touchRead(&touchscreen); 
		keys = keysHeld();		// Verifica el estado del touchscreen
		if (keys & KEY_TOUCH) {
			square_x = (touchscreen.px - 42);
			square_y = (touchscreen.py - 32);
		}

		// Calcula la ventana
		if (square_x < 0) square_x = 0;
		if (square_x > 170) square_x = 170;
		if (square_y < 0) square_y = 0;
		if (square_y > 128) square_y = 128;

		// Resetea el control de zoom
		zoom_x = 0;
		zoom_y = 0;
	
		// Rellena el buffer
		for (y = square_y; y < (square_y + 64); y ++) {
			for (x = square_x; x < (square_x + 85); x ++) {

				// La parte de la izquierda tiene interpolacion bilinear
				if (zoom_x < 128) {

					// Matriz de pixeles
					// 01 A1 A2 02
					// B1 C1 D1
					// B2 D2 C2
					// 03       04

					// Calculos
					// A1 = (1+1+2)/3
					// A2 = (1+2+2)/3
					// B1 = (1+1+3)/3
					// B2 = (1+3+3)/3
					// C1 = (1+1+4)/3
					// C2 = (1+4+4)/3
					// D1 = (2+2+3)/3
					// D2 = (2+3+3)/3

					// Obten el color actual XY (01)
					rgb = NF_16BITS_BACKBUFFER[1][((y << 8) + x)];
					r[1] = (rgb & 0x1F);
					g[1] = ((rgb >> 5) & 0x1F);
					b[1] = ((rgb >> 10) & 0x1F);

					// Obten el color del pixel X + 1 (02)
					if (x < 255) {
						rgb = NF_16BITS_BACKBUFFER[1][((y << 8) + (x + 1))];
					} else {
						rgb = 0;
					}
					r[2] = (rgb & 0x1F);
					g[2] = ((rgb >> 5) & 0x1F);
					b[2] = ((rgb >> 10) & 0x1F);

					// Obten el color del pixel Y + 1 (03)
					if (y < 191) {
						rgb = NF_16BITS_BACKBUFFER[1][(((y + 1) << 8) + x)];
					} else {
						rgb = 0;
					}
					r[3] = (rgb & 0x1F);
					g[3] = ((rgb >> 5) & 0x1F);
					b[3] = ((rgb >> 10) & 0x1F);

					// Obten el color del pixel XY + 1 (04)
					if ((x < 255) && (y < 191)) {
						rgb = NF_16BITS_BACKBUFFER[1][(((y + 1) << 8) + (x + 1))];
					} else {
						rgb = 0;
					}
					r[4] = (rgb & 0x1F);
					g[4] = ((rgb >> 5) & 0x1F);
					b[4] = ((rgb >> 10) & 0x1F);

					// Dibuja la linea divisoria
					if (zoom_x == 126) {
						for (loops = 1; loops < 5; loops ++) {
							r[loops] = 5;
							g[loops] = 10;
							b[loops] = 15;
						}
					}

					// Calcula el valor del pixel 01
					rgb = ((r[1])|((g[1]) << 5)|((b[1]) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + zoom_x)] = rgb;

					// Calcula el pixel del pixel A1
					red = ((r[1] + r[1] + r[2]) / 3);
					green = ((g[1] + g[1] + g[2]) / 3);
					blue = ((b[1] + b[1] + b[2]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + (zoom_x + 1))] = rgb;

					// Calcula el pixel del pixel A2
					red = ((r[1] + r[2] + r[2]) / 3);
					green = ((g[1] + g[2] + g[2]) / 3);
					blue = ((b[1] + b[2] + b[2]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + (zoom_x + 2))] = rgb;

					// Calcula el pixel del pixel B1
					red = ((r[1] + r[1] + r[3]) / 3);
					green = ((g[1] + g[1] + g[3]) / 3);
					blue = ((b[1] + b[1] + b[3]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + zoom_x)] = rgb;

					// Calcula el pixel del pixel B2
					red = ((r[1] + r[3] + r[3]) / 3);
					green = ((g[1] + g[3] + g[3]) / 3);
					blue = ((b[1] + b[3] + b[3]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 2) << 8) + zoom_x)] = rgb;

					// Calcula el pixel del pixel C1
					red = ((r[1] + r[1] + r[4]) / 3);
					green = ((g[1] + g[1] + g[4]) / 3);
					blue = ((b[1] + b[1] + b[4]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + (zoom_x + 1))] = rgb;

					// Calcula el pixel del pixel C2
					red = ((r[1] + r[4] + r[4]) / 3);
					green = ((g[1] + g[4] + g[4]) / 3);
					blue = ((b[1] + b[4] + b[4]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 2) << 8) + (zoom_x + 2))] = rgb;

					// Calcula el pixel del pixel D1
					red = ((r[2] + r[2] + r[3]) / 3);
					green = ((g[2] + g[2] + g[3]) / 3);
					blue = ((b[2] + b[2] + b[3]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + (zoom_x + 2))] = rgb;

					// Calcula el pixel del pixel D2
					red = ((r[2] + r[3] + r[3]) / 3);
					green = ((g[2] + g[3] + g[3]) / 3);
					blue = ((b[2] + b[3] + b[3]) / 3);
					rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 2) << 8) + (zoom_x + 1))] = rgb;

				} else {

					// La parte de la derecha tiene usa pixel doubling

					rgb = NF_16BITS_BACKBUFFER[1][((y << 8) + x)];
					// Escribe los pixeles en el Backbuffer de la pantalla superior 
					// Esto genera el Zoom x2 en la pantalla superior
					NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + zoom_x)] = rgb;		// 01
					NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + (zoom_x + 1))] = rgb;	// A1
					NF_16BITS_BACKBUFFER[0][((zoom_y << 8) + (zoom_x + 2))] = rgb;	// A2
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + zoom_x)] = rgb;	// B1
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 2) << 8) + zoom_x)] = rgb;	// B2
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + (zoom_x + 1))] = rgb;	// C1
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 2) << 8) + (zoom_x + 2))] = rgb;	// C2
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 1) << 8) + (zoom_x + 2))] = rgb;	// D1
					NF_16BITS_BACKBUFFER[0][(((zoom_y + 2) << 8) + (zoom_x + 1))] = rgb;	// D2

				}

				// Desglosa el RGB
				rgb = NF_16BITS_BACKBUFFER[1][((y << 8) + x)];
				red = (rgb & 0x1F);
				green = ((rgb >> 5) & 0x1F);
				blue = ((rgb >> 10) & 0x1F);

				// Genera el equivalente a blanco y negro y sube el brillo 3 puntos
				composite = ((int)((red + green + blue) / 3) + 3);
				if (composite > 31) composite = 31;

				// Reemplaza los valores RGB (efecto marco)
				red = (composite >> 1);
				green = (composite >> 2);
				blue = (composite);

				// Calcula el valor RGB (Alpha + RGB555)
				rgb = ((red)|((green) << 5)|((blue) << 10)|(BIT(15)));

				// Escribelos en el buffer
				NF_16BITS_BACKBUFFER[1][((y << 8) + x)] = rgb;

				// Incrementa dos puntos la X del zoom
				zoom_x += 3;

			}

			zoom_x = 0;		// Resetea la X del zoom
			zoom_y += 3;	// Incrementa dos puntos la Y del zoom

		}

		// Sincronismo Vertical
		swiWaitForVBlank();

		// Copia el contenido del Backbuffer a la VRAM
		NF_Flip16bitsBackBuffer(0);
		NF_Flip16bitsBackBuffer(1);

	}

	return 0; 

}
