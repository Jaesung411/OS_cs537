#include <stdio.h>	// printf
#include <string.h> // memset
#include <stdlib.h> // exit(0);
#include <unistd.h> // close
#include <sys/time.h> //timeval
#include<sys/socket.h>


#include "udp.h"
#include "client.h"


int seq_track=0;

// struct rpc_connection{
//     struct socket recv_socket;
//     struct sockaddr dst_addr;
//     socklen_t dst_len;
//     int seq_number;
//     int client_id;
// };

//fuctionalities
//i) marshals arguments into machine-independent format
//ii) sends request to server
//iii) waits for response
//iv) Unmarshals result and returns to caller


// initializes the RPC connection to the server
/*
/ This function will initialize the rpc_connection struct 
/ and do any other work you need to do to initialize your RPC connection with the server
*/
struct rpc_connection RPC_init(int src_port, int dst_port, char dst_addr[]){

    struct rpc_connection rpc_conn_str;
    rpc_conn_str.recv_socket=init_socket(dst_port);
    rpc_conn_str.dst_addr= *((struct sockaddr*)(&dst_addr));
    rpc_conn_str.dst_len=sizeof(rpc_conn_str.dst_addr);
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    srand(current_time.tv_sec);
    rpc_conn_str.seq_number=seq_track;
    rpc_conn_str.client_id=rand();
    seq_track++;

    // populate_sockaddr(AF_INET, src_port, dst_addr, &(rpc_conn_str.dst_addr), (unsigned int*)rpc_conn_str.dst_len);
    return rpc_conn_str;
}

// Sleeps the server thread for a few seconds
// This function will initiate and block until the idle RPC is completed on the server
void RPC_idle(struct rpc_connection *rpc, int time){
    // while()
    // {   
    //     receive_packet_timeout(rpc->recv_socket,1);
        
    //     send_packet();

    // }
    //ask server to idle; pass rpc related structs and time 

}

// gets the value of a key on the server store
// This function will  initiate and block until the get RPC is completed on the server
int RPC_get(struct rpc_connection *rpc, int key){
    int try=0;
    //send the get request to the RPC server and blocking until response from the server or throwing an exception on no response.
    //Retrying RPC requests on a chosen (short) timeout interval up to 5 times

    //Retrying RPC requests on a chosen (short) timeout interval up to 5 times
    while(try<5)
    {
        //send the request to the server
        send_packet(rpc->recv_socket, &(rpc->dst_addr), rpc->dst_len, &(rpc->seq_number), &(rpc->client_id), sizeof(rpc->client_id), &(key), sizeof(key));

        struct packet_info packet = receive_packet_timeout(rpc->recv_socket, 1);

        //check if client ids and sequence numbers
        if(packet.slen == rpc->dst_len && (*(int*)(packet.buf) == rpc->seq_number))
        {
            if (*(int*)(packet.buf + sizeof(rpc->seq_number)) == key) {
                int value = *(int*)(packet.buf + sizeof(rpc->seq_number) + sizeof(key));
                return value;
            }
        }

        try++;
        if(try >= 5){
            break;
        }
        sleep(1);
        
    }
    perror("Error RPC_get");
    exit(1);
    //Ignore packets that are for other client ids, or to old sequence numbers
    //Delaying retrys for 1 second on receiving an ACK from the server
}

// sets the value of a key on the server store
// This function will  initiate and block until the put RPC is completed on the server
int RPC_put(struct rpc_connection *rpc, int key, int value){

    // int try = 0;
    // struct sockaddr_storage addr;
    // socklen_t addrlen;
    // populate_sockaddr(AF_INET, rpc->dst_addr, rpc->dst_addr, &addr, &addrlen);
    // rpc->dst_addr =*((struct sockaddr*)(&addr));
    // rpc->dst_len = addrlen;

    // struct packet_info packet = receive_packet_timeout(rpc->recv_socket, 1);

    // while(try<5)
    // {
    //     send_packet(rpc->recv_socket,rpc->dst_addr,rpc->dst_len,(char*)&key,sizeof(key));

    //     if(packet.recv_len == 0){
    //         try++;
    //     }
    //     else{
    //         if(try >= 5){
    //             break;
    //         }
    //         sleep(1);
    //     }
    // }
    // perror("Error RPC_put");
    // exit(1);
}

// closes the RPC connection to the server
//  This function will do any cleanup that you need to do for your RPC variables
void RPC_close(struct rpc_connection *rpc){
    close_socket(rpc->recv_socket);
}
