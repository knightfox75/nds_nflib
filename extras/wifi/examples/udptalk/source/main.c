/*
-------------------------------------------------

	NightFox's Lib Template
	Ejemplo basico comunicacion con el protocolo UDP

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por NightFox
	http://www.nightfoxandco.com
	Inicio 10 de Octubre del 2009

	(c)2009 NightFox & Co.

-------------------------------------------------
*/





/*
-------------------------------------------------
	Includes
-------------------------------------------------
*/


// Includes propietarios NDS
#include <nds.h>

// Includes C++
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Includes propios
#include <nf_lib.h>
#include <nf_wifi.h>


/*
-------------------------------------------------
	Defines
-------------------------------------------------
*/

#define myport 22334

char server_ip[18];		// Guarda la IP del servidor

bool _ConnectWIFI(void);			// Conectate al punto de acceso por defecto
u8 _SelectMode(void);				// Servidor o cliente
s16 _SendData(const char* mydata);	// Envia datos por el canal abierto
s16 _GetData(void);					// Recibe datos por el canal abierto
s16 _LookForServer(void);			// Busca un servidor al que conectarse




/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

int main(int argc, char **argv) {

	u8 modo = 0;	// Cliente (1) - Servidor (2)
	u16 keys = 0;	// Lectura del teclado

	u32 contador = 0;
	char temp[256];
	bool loop = true;
	bool senddata = true;

	// Inicializa las 2D
	NF_Set2D(0, 0);				// Modo 2D_0 en la pantalla superior
	NF_Set2D(1, 0);				// Modo 2D_0 en la pantalla inferior

	// Inicializa la consola de texto
	consoleDemoInit();
	consoleClear();

	// Intenta conectarte al WFC...
	if (_ConnectWIFI()) {

		// Selecciona si seras servidor o cliente
		modo = _SelectMode();

		// Inicia como servidor o cliente
		if (modo == 1) {
			// Cliente
			if (_LookForServer() > 0) {
			//if (1) {
				NF_WIFI_CreateUdpSender(server_ip, myport);
			} else {
				loop = false;
			}
		} else {
			// Servidor
			NF_WIFI_CreateUdpListener(myport);
		}



		// Envia o recive datos hasta cancelar la transmision
		while (loop) {

			// Lee el teclado
			scanKeys();
			keys = keysDown();

			if (modo == 1) {	// Envio de datos al servidor

				// Si pulsas A, envia datos
				if (keys & KEY_A) senddata = true;
				// Si pulsas B, pausa el envio
				if (keys & KEY_B) senddata = false;
				// Si pulsas X, resetea el contador
				if (keys & KEY_X) contador = 0;
				// Si se pulsa R, sal
				if (keys & KEY_R) loop = false;



				if (senddata) {

					if (loop) {
						// Debes enviar datos...
						sprintf(temp, "%05d", contador);
					} else {
						// o la señal de salida?
						sprintf(temp, "Exit");
					}

					// Envia y verifica si se han recibido
					iprintf("Envio: %s\n", temp);
					if (_SendData(temp) > 0) {
						// Imprime los datos recibidos
						iprintf("Verificado: %s\n\n", NF_RECV_BUFFER);
						// Incrementa el valor
						contador ++;
						if (contador > 9999) contador = 0;
					} else {
						iprintf("Error de envio.\n");
					}

				}


			} else {	// Recepcion de datos desde el cliente

				// Si se pulsa R, sal
				if (keys & KEY_R) loop = false;

				// Recibe los datos
				if (_GetData() > 0) {
					// Imprime los datos recibidos
					iprintf("Recivido: %s\n", NF_RECV_BUFFER);
					// Si has recibido la señal de salida...
					if (strcmp(NF_RECV_BUFFER, "Exit") == 0) loop = false;
				} else {
					// Si no hay datos, esperalos
					iprintf("Esperando datos...\n");
				}

			}

			// Espera al sincronismo vertical
			swiWaitForVBlank();

		}

	} 

	// Cierra el socket
	close(NF_SOCKET);

	// Deten el WIFI
	NF_WiFiDisconnectAp();

	// Informa
	iprintf("\nSesion finalizada\n");

	while (1) {
		swiWaitForVBlank();
	}


	return 0;

}





/*
-------------------------------------------------
	Funciones
-------------------------------------------------
*/

// Intenta conectarte al punto de acceso por defecto
bool _ConnectWIFI(void) {

	bool status = false;

	// Intenta conectarte al AP...
	iprintf("Conectando via WFC...\n\n");
	swiWaitForVBlank();

	if (NF_WiFiConnectDefaultAp()) {

		// Devuelve los parametros de la conexion TCP/IP con el AP
		iprintf("Conexion realizada.\n\n");
		iprintf("Ip     : %s\n", inet_ntoa(NF_IP));
		iprintf("Gateway: %s\n", inet_ntoa(NF_GATEWAY));
		iprintf("Mask   : %s\n", inet_ntoa(NF_MASK));
		iprintf("Dns1   : %s\n", inet_ntoa(NF_DNS1));
		iprintf("Dns2   : %s\n\n", inet_ntoa(NF_DNS2));
		
		// Marca como conectado
		status = true;

	} else {

		// No se ha podido conectar
		iprintf("Conexion no establecida\n");

	}

	swiWaitForVBlank();
	return status;

}



// Selecciona si seras Cliente (1) o Servidor (2)
u8 _SelectMode(void) {

	u8 modo = 0;
	u16 keys = 0;

	iprintf("A - Cliente\nB - Servidor\n");
	swiWaitForVBlank();

	while (modo == 0) {
		scanKeys();
		keys = keysDown();
		if (keys & KEY_A) modo = 1;
		if (keys & KEY_B) modo = 2;
		swiWaitForVBlank();
	}

	return modo;

}



// Envia datos por el canal abierto
s16 _SendData(const char* mydata) {

	// Variables
	bool checksum = false;
	bool timeout = false;
	s32 status = 0;
	char buffer[NF_WIFI_BUFFER_SIZE];

	// Copia los datos al buffer
	sprintf(buffer, "%s", mydata);

	while (!checksum && !timeout) {
		// Envia los datos por UDP
		NF_WIFI_UdpSend(buffer);
		// Espera la confirmacion de los datos
		status = NF_WIFI_UdpListen(600000);	// Time out 600 milisegundos
		// Realiza un checksum de los datos
		if (status > 0) {
			if ((strcmp(NF_SEND_BUFFER, NF_RECV_BUFFER) == 0)) {
				// Si el envio es correcto, continua
				checksum = true;
			} else {
				checksum = true;
				status = -1;	// Error de checksum
			}
		} else {
			timeout = true;
			status = -2;		// Time out
		}
	}

	// Devuelve el estado
	return status;

}



// Recibe datos por el canal abierto
s16 _GetData(void) {

	// Variables
	s32 status = 0;

	// Espera los datos
	status = NF_WIFI_UdpListen(300000);	// Time out 300 milisegundos

	// Si se han recibido datos, devuelvelos para su verificacion
	if (status > 0) NF_WIFI_UdpSend(NF_RECV_BUFFER);

	// Devuelve el estado
	return status;

}



// Escanea el rango de IP's para encontrar el servidor
s16 _LookForServer(void) {

	// Variables locales
	char myip[18];
	char temp[18];
	char character[2];

	u16 n = 0;
	u16 p = 0;

	bool loop = true;
	s8 status = 0;

	u16 keys = 0;

	// Guarda la direccion IP local (su rango, por ejemplo "192.168.0."
	sprintf(temp, "%s", inet_ntoa(NF_IP));
	for (n = 0; n < (strlen(temp)); n ++) {
		// Copia el caracter
		memcpy((myip + n), (temp + n), 1);
		memcpy(character, (temp + n), 1);
		character[1] = '\0';
		// Si es un punto...
		if (strcmp(character, ".") == 0) {
			p ++;
			// Si es el 3er punto...
			if (p == 3) {
				myip[(n + 1)] = '\0';	// Pon el caracter terminador
				n = strlen(temp);		// Y termina
			}
		}
	}

	// Escanea todo el rango de IP's
	p = 0;

	for (n = 1; n < 255; n ++) {

		// Lee el teclado
		scanKeys();
		keys = keysDown();

		// Calcula la IP
		sprintf(temp, "%s%d", myip, n);
		iprintf("%s   ", temp);
		swiWaitForVBlank();

		// Crea el socket de envio
		NF_WIFI_CreateUdpSender(temp, myport);

		// Envia y verifica si se han recibido
		loop = true;
		while (loop) {
			status = _SendData("HANDSHAKE");
			if (status > 0) {
				// Imprime los datos recibidos
				iprintf("Encontrado.\n");
				// Guarda el valor
				sprintf(server_ip, temp);
				p = n;
				n = 255;
				loop = false;
			} else {
				iprintf("Time Out.\n");
				if (status == -2) loop = false;
			}
		}

		// Cierra el socket creado
		close(NF_SOCKET);

		// Si pulsas cancelar
		if (keys & KEY_R) n = 255;
		
	}

	return p;

}
