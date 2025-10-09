// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: NightFox & Co., 2010
//
// Basic UDP communications example.
// http://www.nightfoxandco.com

#include <stdio.h>
#include <unistd.h>

#include <nds.h>

#include <nf_lib.h>
#include <nf_wifi.h>

#define myport 22334

char server_ip[18]; // Stores the IP of the server

// Tries to connect to the default access point
bool ConnectWIFI(void)
{
    bool status = false;

    printf("Connecting to WFC...\n\n");
    cothread_yield_irq(IRQ_VBLANK);

    if (NF_WiFiConnectDefaultAp())
    {
        // Return connection parameters
        printf("Connected to access point\n\n");
        printf("IP     : %s\n", inet_ntoa(NF_IP));
        printf("Gateway: %s\n", inet_ntoa(NF_GATEWAY));
        printf("Mask   : %s\n", inet_ntoa(NF_MASK));
        printf("DNS1   : %s\n", inet_ntoa(NF_DNS1));
        printf("DNS2   : %s\n\n", inet_ntoa(NF_DNS2));

        // Success
        status = true;
    }
    else
    {
        // Error
        printf("Failed to connect\n");
    }

    cothread_yield_irq(IRQ_VBLANK);
    return status;
}

// Wait for the user to select client (1) or server (2) mode.
u8 SelectMode(void)
{
    u8 modo = 0;

    printf("A - Client\nB - Server\n");
    cothread_yield_irq(IRQ_VBLANK);

    while (modo == 0)
    {
        scanKeys();
        u16 keys = keysDown();

        if (keys & KEY_A)
            modo = 1;
        if (keys & KEY_B)
            modo = 2;

        cothread_yield_irq(IRQ_VBLANK);
    }

    return modo;
}

// Send data through the open channel
s16 SendData(const char *mydata)
{
    bool checksum = false;
    bool timeout = false;
    s32 status = 0;
    char buffer[NF_WIFI_BUFFER_SIZE];

    // Copy data to a temporary buffer
    sprintf(buffer, "%s", mydata);

    while (!checksum && !timeout)
    {
        // Send data using UDP
        NF_WIFI_UdpSend(buffer);

        // Wait for the confirmation from the other end with a 600 ms timeout
        status = NF_WIFI_UdpListen(600000);

        // Do a data checksum
        if (status > 0)
        {
            if ((strcmp(NF_SEND_BUFFER, NF_RECV_BUFFER) == 0))
            {
                checksum = true;
            }
            else
            {
                checksum = true;
                status = -1; // Checksum error
            }
        }
        else
        {
            timeout = true;
            status = -2; // Timed out
        }
    }

    return status;
}

// Receive data from the open channel
s16 GetData(void)
{
    // Listen to data with a 300 millisecond timeout
    s32 status = NF_WIFI_UdpListen(300000);

    // If any data has been received, return them to the sender to check them
    if (status > 0)
        NF_WIFI_UdpSend(NF_RECV_BUFFER);

    return status;
}

// Scans all IPs in the specified range to look for a server to connect to.
s16 LookForServer(void)
{
    // Variables locales
    char myip[18];
    char temp[64];

    u16 p = 0;

    bool loop = true;
    s8 status = 0;

    // Get the local IP and extract its range (for example, "192.168.0.")
    sprintf(temp, "%s", inet_ntoa(NF_IP));
    for (int n = 0; n < strlen(temp); n++)
    {
        myip[n] = temp[n];
        char c = temp[n];

        if (c == '.')
        {
            p++;

            // If this is the third dot, end the string
            if (p == 3)
            {
                myip[n + 1] = '\0';
                break;
            }
        }
    }

    // Scan all possible IPs
    p = 0;

    for (int n = 1; n < 255; n++)
    {
        scanKeys();
        u16 keys = keysDown();

        // Generate IP
        sprintf(temp, "%s%d", myip, n);
        printf("%s   ", temp);
        cothread_yield_irq(IRQ_VBLANK);

        // Create sender socket
        NF_WIFI_CreateUdpSender(temp, myport);

        // Send data and verify that it has been received
        loop = true;
        while (loop)
        {
            status = SendData("HANDSHAKE");
            if (status > 0)
            {
                // Print received data
                printf("Found.\n");

                // Save its value
                sprintf(server_ip, temp);
                p = n;
                n = 255;
                loop = false;
            }
            else
            {
                printf("Timed out.\n");
                if (status == -2)
                    loop = false;
            }
        }

        // Close the socket
        closesocket(NF_SOCKET);

        // If the user wants to cancel, exit
        if (keys & KEY_R)
            break;
    }

    return p;
}

int main(int argc, char **argv)
{
    u8 modo = 0;    // Client (1) - Server (2)

    u32 counter = 0;
    char temp[256];
    bool loop = true;
    bool senddata = true;

    // Initialize 2D engine in both screens and use mode 0
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    // Initialize demo text console
    consoleDemoInit();

    // Try to connect to the WiFi access point
    if (ConnectWIFI())
    {
        // Ask the user if this console is a server or a client
        modo = SelectMode();

        if (modo == 1)
        {
            // Run as client
            if (LookForServer() > 0)
                NF_WIFI_CreateUdpSender(server_ip, myport);
            else
                loop = false;
        }
        else
        {
            // Run as server
            NF_WIFI_CreateUdpListener(myport);
        }

        // Send or receive data until the transmission is cancelled
        while (loop)
        {
            scanKeys();
            u16 keys = keysDown();

            if (modo == 1)
            {
                // Send data to server

                // Send data when the user presses A
                if (keys & KEY_A)
                    senddata = true;

                // Stop sending data when the user presses B
                if (keys & KEY_B)
                    senddata = false;

                // Reset counter when the user presses X
                if (keys & KEY_X)
                    counter = 0;

                // Exit if R is pressed
                if (keys & KEY_R)
                    loop = false;

                if (senddata)
                {
                    if (loop)
                    {
                        // Send data if the loop is active
                        sprintf(temp, "%05lu", counter);
                    }
                    else
                    {
                        // If not, send an exit string
                        sprintf(temp, "Exit");
                    }

                    // Send it and verify that it has been received
                    printf("Sent: %s\n", temp);
                    if (SendData(temp) > 0)
                    {
                        // Print the received data
                        printf("Verified: %s\n\n", NF_RECV_BUFFER);
                        // Increment counter
                        counter++;
                        if (counter > 9999)
                            counter = 0;
                    }
                    else
                    {
                        printf("Send error.\n");
                    }
                }
            }
            else
            {
                // Data reception in the client

                // Exit loop if R is pressed
                if (keys & KEY_R)
                    loop = false;

                // Receive data
                if (GetData() > 0)
                {
                    // Print received data
                    printf("Received: %s\n", NF_RECV_BUFFER);

                    // If the exit signal has been received
                    if (strcmp(NF_RECV_BUFFER, "Exit") == 0)
                        loop = false;
                }
                else
                {
                    // If there is nothing available, wait for data
                    printf("Waiting for data...\n");
                }
            }

            // Wait for the screen refresh
            cothread_yield_irq(IRQ_VBLANK);
        }
    }

    // Close the socket
    closesocket(NF_SOCKET);

    // Stop WiFi
    NF_WiFiDisconnectAp();

    printf("\nTest complete\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);
    }

    return 0;
}
