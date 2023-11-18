#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "udp.h"
#include "server_functions.h"

#define SERVER_PORT 8888

int main(void)
{
    // struct sockaddr_in client_addr;
    struct packet_info packet;

    struct socket server_socket = init_socket(SERVER_PORT);

    printf("Server started on port %d\n", SERVER_PORT);

    while (1)
    {
        packet = receive_packet(server_socket);

        int key = atoi(packet.buf);
        int value = get(key);

        // process request
        if (value == -1)
        {
            send_packet(server_socket, packet.sock, packet.slen, "Key not found", strlen("Key not found"));
        }
        else
        {
            char buffer[BUFLEN];
            snprintf(buffer, BUFLEN, "Key %d has value %d", key, value);
            send_packet(server_socket, packet.sock, packet.slen, buffer, strlen(buffer));
        }
    }

    close_socket(server_socket);

    return 0;
}
