#ifndef __NF_WIFI_H__
#define __NF_WIFI_H__





// NightFox LIB - Include de funciones de WI-FI
// Requiere DevkitARM
// Codigo por NightFox
// http://www.nightfoxandco.com/
// Version 20110502





// Includes devKitPro
#include <nds.h>
#include <dswifi9.h>

// Includes C
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>





// Defines de la red
#define NF_MAX_INCOMMING_PENDING 1		// Peticiones de conexion maximas en cola
#define NF_WIFI_BUFFER_SIZE 256			// Tamaño del buffer de comunicaciones

// Parametros de la RED
extern struct in_addr NF_IP, NF_GATEWAY, NF_MASK, NF_DNS1, NF_DNS2;		// Datos de la LAN

// Estructura del socket
extern s32 NF_SOCKET;			// Id del socket (servidor)
extern s32 NF_CONNECTED;		// Resultado de la conexion
extern s32 NF_SINSIZE;			// Tamaño de la Struct .SIN
extern s32 NF_BYTES_RECIEVED;	// Bytes recibidos

extern struct sockaddr_in NF_SA_SERVER;		// Estructura Socket Adress In (Servidor)
extern struct sockaddr_in NF_SA_CLIENT;		// Estructura Socket Adress In (Cliente)

extern char NF_SEND_BUFFER[NF_WIFI_BUFFER_SIZE];	// Buffer de envio
extern char NF_RECV_BUFFER[NF_WIFI_BUFFER_SIZE];	// Buffer de recepcion

extern bool NF_WIFI_IS_SERVER;	// Almacena si eres servidor o cliente

extern s32 NF_MAXFD;				// Numero maximo de sockets a examinar por select();
extern fd_set NF_READFDS;			// Estructura donde se almacenaran los datos de los sockets para select();
extern struct timeval NF_TIMEOUT;	// Almacena el valor del time out






// Funcion NF_WiFiConnectDefaultAp();
extern bool NF_WiFiConnectDefaultAp(void);
// Intenta conectarte al punto de acceso WI-FI definido por defecto en la WFC de la DS
// Devuelve si se ha conseguido la conexion



// Funcion NF_WiFiDisconnectAp();
void NF_WiFiDisconnectAp(void);
// Desconectate del punto de acceso y cierra la conexion WI-FI



// Function NF_WIFI_CreateUdpSender();
extern bool NF_WIFI_CreateUdpSender(const char* address, u16 port);
// Crea un socket UDP en el puerto especificado para mandar datos
// a la direccion IP especificada



// Function NF_WIFI_CreateUdpListener();
extern bool NF_WIFI_CreateUdpListener(u16 port);
// Crea un socket UDP en el puerto especificado y dejalo
// a la escucha



// Funcion NF_WIFI_UdpSend();
bool NF_WIFI_UdpSend(const char* data);
// Envia una cadena de datos por el puerto UDP previamente abierto
// por NF_WIFI_CreateUdpSender();



// Funcion NF_WIFI_UdpListen();
s32 NF_WIFI_UdpListen(u32 timeout);
// Lee los datos del puerto UDP y ponlos en el buffer.





#endif
