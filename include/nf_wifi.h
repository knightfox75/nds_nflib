// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Wi-Fi functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_WIFI_H__
#define NF_WIFI_H__

/// @file   nf_wifi.h
/// @brief  Basic WiFi helpers

/// @defgroup nf_wifi Basic WiFi helpers
///
/// Basic functions to initialize WiFi, setup a UDP socket, and send and receive
/// data.
///
/// @{

#include <nds.h>
#include <dswifi9.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/// Maxmimum number of connection requests in the queue
#define NF_MAX_INCOMMING_PENDING 1

/// Size of communications buffer
#define NF_WIFI_BUFFER_SIZE 256

/// Current IP address
extern struct in_addr NF_IP;

/// Current gateway
extern struct in_addr NF_GATEWAY;

/// Current netmask
extern struct in_addr NF_MASK;

/// Current DNS 1
extern struct in_addr NF_DNS1;

/// Current DNS 2
extern struct in_addr NF_DNS2;


/// Server socket ID
extern s32 NF_SOCKET;

/// Connection result
extern s32 NF_CONNECTED;

/// Size of struct .SIN
extern int NF_SINSIZE;

/// Received bytes
extern s32 NF_BYTES_RECIEVED;


/// Server socket address
extern struct sockaddr_in NF_SA_SERVER;
/// Client socket address
extern struct sockaddr_in NF_SA_CLIENT;


/// Send buffer
extern char NF_SEND_BUFFER[NF_WIFI_BUFFER_SIZE];
/// Reception buffer
extern char NF_RECV_BUFFER[NF_WIFI_BUFFER_SIZE];


/// This is set to true if this console is in server mode, false in client mode
extern bool NF_WIFI_IS_SERVER;


/// Maximum number of sockets to examine by select().
extern s32 NF_MAXFD;

/// Struct to save data used by select() sockets.
extern fd_set NF_READFDS;

/// Current timeout in microseconds
extern struct timeval NF_TIMEOUT;


/// Try to connect to the default access point in the DS user configuration.
///
/// @return True if the connection has succeeded.
bool NF_WiFiConnectDefaultAp(void);

/// Disconnect from the access point and shutdown WiFi.
void NF_WiFiDisconnectAp(void);

/// Creates a socket in client mode using UDP.
///
/// Example:
/// ```
/// // Create a UDP socket with address 192.168.1.201 and port 12345
/// NF_WIFI_CreateUdpSender("192.168.1.201", 12345);
/// ```
///
/// @param address IP address.
/// @param port Port.
/// @return True if it has been created successfully
bool NF_WIFI_CreateUdpSender(const char *address, u16 port);

/// Creates a socket in server mode using UDP.
///
/// It accepts data from any UP address.
///
/// Example:
/// ```
/// // Create a UDP socket at port 12345
/// NF_WIFI_CreateUdpListener(12345);
/// ```
///
/// @param port Port.
/// @return True if it has been created successfully
bool NF_WIFI_CreateUdpListener(u16 port);

/// Sends a text string over the currenty open UDP socket.
///
/// A socket must have been opened by NF_WIFI_CreateUdpSender(), which will be
/// used by this function.
///
/// Sent data is saved in NF_SEND_BUFFER as well so that you can run a checksum
/// function on it, for example.
///
/// Example:
/// ```
/// // Sends string "This is a test" over the currently open socket
/// NF_WIFI_UdpSend("This is a test");
/// ```
///
/// @param data Text string to be sent.
/// @return True on success.
bool NF_WIFI_UdpSend(const char *data);

/// Listens to data from the currently open socket.
///
/// If any data is received it is saved in the global variable NF_RECV_BUFFER.
/// It saves the number of received bytes in NF_BYTES_RECIEVED.
///
/// The function returns a positive value if any data is received. If there is a
/// timeout (specified in microseconds), it returns a negative value.
///
/// Example:
/// ```
/// // Listen for data for with a timeout of 300000 microseconds (300 ms)
/// NF_WIFI_UdpListen(300000);
/// ```
///
/// @param timeout
/// @return Positive number on success, negative number on error.
s32 NF_WIFI_UdpListen(u32 timeout);

/// @}

#endif // NF_WIFI_H__

#ifdef __cplusplus
}
#endif
