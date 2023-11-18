// Track up to 100 connected clients
// Excecute idle, get, and put commands (given in the starter code)
// Receiving packets from a client handling the execution
// Execute the requested function if the sequence number is greater than what the server has tracked for the client. This should also begin running a new thread that will allow for multiple clients to connect at the same time
// Resend results from the most recent RPC for every client if the sequence number is equal
// Reply with an acknowlegement (ACK) for in progress requests
// Ignore requests that are older than the most recent requests sequence number 


//functionalities
//i) Unmarshals arguments and builds stack frame

//ii) Calles procedure

//iii)Server stub marshals results and sends reply

#include "server_functions.h"
#include <stdio.h>	// printf
#include <string.h> // memset
#include <stdlib.h> // exit(0);
#include <unistd.h> // close
#include <sys/time.h> //timeval

#include "udp.h"

int track_clients=0;

int main(int argc, char *argv[])
{
	// int port = argv[1];

    // while(1) {
	//     struct packet_info packet = receive_packet(sock);
    //     // if(packet_temp->recv_len!=-1)
    //     // }
    //     if(packet_temp->slen!=-1)
    //     {
    //         //print error message
    //     }
	//     //... other code
    // }

    int port = atoi(argv[1]);

    // struct sockaddr_in client_addr;

    struct socket server_socket = init_socket(port);

    while (1)
    {
        // struct packet_info packet = receive_packet(server_socket);

        // int key = atoi(packet.buf);
        // int value = get(key);

    }

    close_socket(server_socket);

}