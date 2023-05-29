// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Wi-Fi functions
// http://www.nightfoxandco.com/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <nds.h>
#include <dswifi9.h>

#include "nf_wifi.h"

// Network parameters
struct in_addr NF_IP, NF_GATEWAY, NF_MASK, NF_DNS1, NF_DNS2;

// Socket information
s32 NF_SOCKET;          // ID of the server socket
s32 NF_CONNECTED;       // Result of the connection
int NF_SINSIZE;         // Size of the .SIN struct
s32 NF_BYTES_RECIEVED;  // Received bytes

struct sockaddr_in NF_SA_SERVER; // Socket address structure (server)
struct sockaddr_in NF_SA_CLIENT; // Socket address structure (client)

char NF_SEND_BUFFER[NF_WIFI_BUFFER_SIZE]; // Send buffer
char NF_RECV_BUFFER[NF_WIFI_BUFFER_SIZE]; // Receive buffer

bool NF_WIFI_IS_SERVER; // Keeps track of whether you're a server or client

s32 NF_MAXFD;       // Max number of sockets to examine with select()
fd_set NF_READFDS;  // Struct where socket data is stored for select()
struct timeval NF_TIMEOUT; // Stores the value of the timeout

bool NF_WiFiConnectDefaultAp(void)
{
    // Try to connect to the default access point
    if (Wifi_InitDefault(WFC_CONNECT))
    {
        // Get connection information
        NF_IP = Wifi_GetIPInfo(&NF_GATEWAY, &NF_MASK, &NF_DNS1, &NF_DNS2);
        return true;
    }

    return false;
}

void NF_WiFiDisconnectAp(void)
{
    // Disconnect from the access point and disable the WiFI hardware
    Wifi_DisconnectAP();
    Wifi_DisableWifi();
}

bool NF_WIFI_CreateUdpSender(const char *address, u16 port)
{
    // Get host IP
    char ip[24];
    sprintf(ip, "%s", address);

    // Cleanup sockets before starting
    memset(&NF_SA_SERVER, 0, sizeof(NF_SA_SERVER));
    memset(&NF_SA_CLIENT, 0, sizeof(NF_SA_CLIENT));

    // Tell the socket where to connect

    // IPv4 internet connection
    NF_SA_SERVER.sin_family = AF_INET;

    // Specify the port in network byte order
    NF_SA_SERVER.sin_port = htons(port);

    // Specify the IP address in the right format (converted from ASCII)
    NF_SA_SERVER.sin_addr.s_addr = inet_addr(ip);

    // Try to create socket
    if ((NF_SOCKET = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return false;

    // Save that this DS is acting as a client
    NF_WIFI_IS_SERVER = false;

    // Return success
    return true;
}

bool NF_WIFI_CreateUdpListener(u16 port)
{
    // Cleanup sockets before starting
    memset(&NF_SA_SERVER, 0, sizeof(NF_SA_SERVER));
    memset(&NF_SA_CLIENT, 0, sizeof(NF_SA_CLIENT));

    // IPv4 internet connection
    NF_SA_SERVER.sin_family = AF_INET;

    // Specify the port in network byte order
    NF_SA_SERVER.sin_port = htons(port);

    // Specify the IP address in the right format (converted from ASCII).
    // INADDR_ANY means that, as a server, it accepts connections from any IP
    // address.
    NF_SA_SERVER.sin_addr.s_addr = htonl(INADDR_ANY);

    // Try to create socket
    if ((NF_SOCKET = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return false;

    // Bind socket to the specified port
    if ((bind(NF_SOCKET, (struct sockaddr *)&NF_SA_SERVER, sizeof(NF_SA_SERVER))) == -1)
        return false;

    // Save size of struct sockaddr
    NF_SINSIZE = sizeof(struct sockaddr);

    // Save that this DS is acting as a server
    NF_WIFI_IS_SERVER = true;

    return true;
}

bool NF_WIFI_UdpSend(const char *data)
{
    // Clear send buffer and copy the string being sent
    memset(&NF_SEND_BUFFER, 0, sizeof(NF_SEND_BUFFER));
    snprintf(NF_SEND_BUFFER, sizeof(NF_SEND_BUFFER), "%s", data);

    // Send data through the currently open UDP port
    if (NF_WIFI_IS_SERVER)
    {
        // If this DS is a server, it is in listening mode. Send the string to
        // the last IP where it received data from.
        sendto(NF_SOCKET, NF_SEND_BUFFER, strlen(NF_SEND_BUFFER), 0,
               (struct sockaddr *)&NF_SA_CLIENT, sizeof(struct sockaddr));
    }
    else
    {
        // Send data to the IP of the server (this DS is in sender mode).
        sendto(NF_SOCKET, NF_SEND_BUFFER, strlen(NF_SEND_BUFFER), 0,
               (struct sockaddr *)&NF_SA_SERVER, sizeof(struct sockaddr));
    }

    // Return success
    return true;
}

s32 NF_WIFI_UdpListen(u32 timeout)
{
    // Clear the previous result of select()
    FD_ZERO(&NF_READFDS);

    // Specify the listener socket
    FD_SET(NF_SOCKET, &NF_READFDS);

    // Calculate the maximum value to listen to
    NF_MAXFD = NF_SOCKET + 1;

    // Specify the timeout
    NF_TIMEOUT.tv_sec = 0;
    NF_TIMEOUT.tv_usec = timeout;

    // Select this socket
    s32 status = select(NF_MAXFD, &NF_READFDS, NULL, NULL, &NF_TIMEOUT);

    // If there is data available to be read
    if ((status > 0) && FD_ISSET(NF_SOCKET, &NF_READFDS))
    {
        // Clear the previous value of the reception buffer
        memset(&NF_RECV_BUFFER, 0, sizeof(NF_RECV_BUFFER));

        // Receive data and save it to the buffer, but leave the last byte
        // unused to be able to store a '\0'
        NF_BYTES_RECIEVED = recvfrom(NF_SOCKET, NF_RECV_BUFFER, NF_WIFI_BUFFER_SIZE - 1,
                                     0, (struct sockaddr *)&NF_SA_CLIENT, &NF_SINSIZE);
        NF_RECV_BUFFER[NF_BYTES_RECIEVED] = '\0';
    }

    // Return result
    return status;
}
