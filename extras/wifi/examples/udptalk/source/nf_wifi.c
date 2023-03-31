
// NightFox LIB - Funciones de WI-FI
// Requiere DevkitARM
// Codigo por NightFox
// http://www.nightfoxandco.com/
// Version 20110502





// Includes devKitPro
#include <nds.h>
#include <dswifi9.h>

// Includes C
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Includes propios
#include "nf_wifi.h"





// Parametros de la RED
struct in_addr NF_IP, NF_GATEWAY, NF_MASK, NF_DNS1, NF_DNS2;		// Datos de la LAN

// Estructura del socket
s32 NF_SOCKET;			// Id del socket (servidor)
s32 NF_CONNECTED;		// Resultado de la conexion
s32 NF_SINSIZE;			// Tamaño de la Struct .SIN
s32 NF_BYTES_RECIEVED;	// Bytes recibidos

struct sockaddr_in NF_SA_SERVER;		// Estructura Socket Adress In (Servidor)
struct sockaddr_in NF_SA_CLIENT;		// Estructura Socket Adress In (Cliente)

char NF_SEND_BUFFER[NF_WIFI_BUFFER_SIZE];	// Buffer de envio
char NF_RECV_BUFFER[NF_WIFI_BUFFER_SIZE];	// Buffer de recepcion

bool NF_WIFI_IS_SERVER;	// Almacena si eres servidor o cliente

s32 NF_MAXFD;				// Numero maximo de sockets a examinar por select();
fd_set NF_READFDS;			// Estructura donde se almacenaran los datos de los sockets para select();
struct timeval NF_TIMEOUT;	// Almacena el valor del time out





// Funcion NF_WiFiConnectDefaultAp();
bool NF_WiFiConnectDefaultAp(void) {

	// Variables locales
	bool connect = false;

	// Intenta conectarte al punto de acceso por defecto
	if (Wifi_InitDefault(WFC_CONNECT)) {
		// Obten los datos de la conexion
		NF_IP = Wifi_GetIPInfo(&NF_GATEWAY, &NF_MASK, &NF_DNS1, &NF_DNS2);
		connect = true;
	}

	// Devuelve el resultado
	return connect;

}



// Funcion NF_WiFiDisconnectAp();
void NF_WiFiDisconnectAp(void) {

	// Desconectate del punto de acceso
	Wifi_DisconnectAP();
	// Y apagala WIFI
	Wifi_DisableWifi();

}



// Function NF_WIFI_CreateUdpSender();
bool NF_WIFI_CreateUdpSender(const char* address, u16 port) {

	// Variables locales
	bool status = true;
	char ip[24];

	// Obtiene la direccion de host
	sprintf(ip, "%s", address);

	// Seguidamente, comunicale al Socket donde debe conectarse
	// *** Vacia la estructura antes que nada ***
	memset(&NF_SA_SERVER, 0, sizeof(NF_SA_SERVER));
	memset(&NF_SA_CLIENT, 0, sizeof(NF_SA_CLIENT));
	// *** Tipo de conexion *** "  .sin_family = AF_INET" especifica que el tipo de socket IPv4 Internet.
	NF_SA_SERVER.sin_family = AF_INET;	
	// *** Puerto de conexion ***  "htons()" convierte el valor de u16 a "TCP/IP network byte order" 
	NF_SA_SERVER.sin_port = htons(port);
	// *** Direccion IP de la conexion ***
	// "inet_addr()" convierte una direccion IPv4 en formato texto ("192.168.0.1") al formato IN_ADDR
	// "inet_ntoa()" convierte al formato texto la direccion almacenada en "struct sockaddr_in"
	NF_SA_SERVER.sin_addr.s_addr = inet_addr(ip);

	// Crea el socket
	if ((NF_SOCKET = socket(AF_INET, SOCK_DGRAM, 0)) == -1) status = false;

	// Marca esta DS como cliente (envia los datos a una IP conocida [SERVIDOR])
	NF_WIFI_IS_SERVER = false;

	// Devuelve el estado de la funcion
	return status;

}



// Function NF_WIFI_CreateUdpListener();
bool NF_WIFI_CreateUdpListener(u16 port) {

	// Variables locales
	bool status = true;

	// *** Vacia la estructura antes que nada ***
	memset(&NF_SA_SERVER, 0, sizeof(NF_SA_SERVER));
	memset(&NF_SA_CLIENT, 0, sizeof(NF_SA_CLIENT));
	// *** Tipo de conexion *** "  .sin_family = AF_INET" especifica que el tipo de socket IPv4 Internet.
	NF_SA_SERVER.sin_family = AF_INET;	
	// *** Puerto de conexion ***  "htons()" convierte el valor de u16 a "TCP/IP network byte order" 
	NF_SA_SERVER.sin_port = htons(port);
	// *** Direccion IP de la conexion ***
	// "inet_addr()" convierte una direccion IPv4 en formato texto ("192.168.0.1") al formato IN_ADDR
	// "inet_ntoa()" convierte al formato texto la direccion almacenada en "struct sockaddr_in"
	// "INADDR_ANY" como servidor, acepta conexiones desde cualquier IP
	NF_SA_SERVER.sin_addr.s_addr = htonl(INADDR_ANY);

	// Crea el socket
	if ((NF_SOCKET = socket(AF_INET, SOCK_DGRAM, 0)) == -1) status = false;

	// Enlaza el socket a un puerto concreto
	if ((bind(NF_SOCKET, (struct sockaddr*) &NF_SA_SERVER, sizeof(NF_SA_SERVER))) == -1) status = false;

	// Tamaño del struct sockaddr
	NF_SINSIZE = sizeof(struct sockaddr);

	// Marca esta DS como servidor (recibe datos desde cualquier IP [CLIENTE])
	NF_WIFI_IS_SERVER = true;

	return status;

}



// Funcion NF_WIFI_UdpSend();
bool NF_WIFI_UdpSend(const char* data) {

	// Borra el buffer de envio
	memset(&NF_SEND_BUFFER, 0, sizeof(NF_SEND_BUFFER));

	// Pon los datos en el buffer de envio
	sprintf(NF_SEND_BUFFER, "%s", data);

	// Envia los datos a traves del puerto UDP abierto
	if (NF_WIFI_IS_SERVER) {
		// Envia los datos a la IP desde la cual has recibido datos (esta DS esta en modo LISTENER)
		sendto(NF_SOCKET, NF_SEND_BUFFER, strlen(NF_SEND_BUFFER), 0, (struct sockaddr *)&NF_SA_CLIENT, sizeof(struct sockaddr));
	} else {
		// Envia los datos a la IP del servidor (esta DS esta en modo SENDER)
		sendto(NF_SOCKET, NF_SEND_BUFFER, strlen(NF_SEND_BUFFER), 0, (struct sockaddr *)&NF_SA_SERVER, sizeof(struct sockaddr));
	}

	// Devuelte el estado
	return true;

}



// Funcion NF_WIFI_UdpListen();
s32 NF_WIFI_UdpListen(u32 timeout) {

	s32 status = 0;

	// Borra el estado de select();
	FD_ZERO(&NF_READFDS);

	// Asigna el socket a la escucha
	FD_SET(NF_SOCKET, &NF_READFDS);

	// Calcula el valor maximo a escuchar
	NF_MAXFD = (NF_SOCKET + 1);

	// Y ahora aplica el timeout
	NF_TIMEOUT.tv_sec = 0;
	NF_TIMEOUT.tv_usec = timeout;

	// Ejecuta el comando select
	status = select(NF_MAXFD, &NF_READFDS, NULL, NULL, &NF_TIMEOUT);

	// Si hay datos disponibles para ser leidos
	if ((status > 0) && FD_ISSET(NF_SOCKET, &NF_READFDS)) {

		// Borra el buffer de recepcion
		memset(&NF_RECV_BUFFER, 0, sizeof(NF_RECV_BUFFER));

		// Recive los datos y ponlos en el buffer
		NF_BYTES_RECIEVED = recvfrom(NF_SOCKET, NF_RECV_BUFFER, NF_WIFI_BUFFER_SIZE, 0, (struct sockaddr *)&NF_SA_CLIENT, &NF_SINSIZE);

		// Marca el final del buffer de recepcion
		NF_RECV_BUFFER[NF_BYTES_RECIEVED] = '\0';

		// Fuerza el vaciado de los buffers
		fflush(stdout);

	}

	// Devuelte el estado
	return status;

}
