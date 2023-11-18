#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void die(char *s) {
    perror(s);
    exit(1);
}


struct rpc_connection RPC_init(int src_port, int dst_port, char dst_addr[]) {
    struct rpc_connection rpc;

    // Create the socket
    rpc.recv_socket = init_socket(src_port);

    // Populate the destination address structure
    populate_sockaddr(AF_INET, dst_port, dst_addr, (struct sockaddr_storage *)&rpc.dst_addr, &rpc.dst_len);

    // Set initial sequence number and client ID
    rpc.seq_number = 0;
    rpc.client_id = rand() % 65536;

    return rpc;
}

void RPC_idle(struct rpc_connection *rpc, int time) {
    idle(time);
}

int RPC_get(struct rpc_connection *rpc, int key) {
    struct packet_info packet;
    int retries = 0;

    // Create the request packet
    struct rpc_packet request_packet;
    request_packet.client_id = rpc->client_id;
    request_packet.seq_number = rpc->seq_number++;
    request_packet.opcode = RPC_OPCODE_GET;
    request_packet.key = key;

    while (retries < RETRY_COUNT) {
        // Send the request packet to the server
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, (char *)&request_packet, sizeof(struct rpc_packet));

        // Wait for a response packet
        packet = receive_packet_timeout(rpc->recv_socket, TIMEOUT_TIME);

        // Check if the response packet is valid
        if (packet.recv_len == sizeof(struct rpc_packet)) {
            struct rpc_packet *response_packet = (struct rpc_packet *)packet.buf;

            if (response_packet->client_id == rpc->client_id &&
                response_packet->seq_number == request_packet.seq_number &&
                response_packet->opcode == RPC_OPCODE_GET_RESPONSE) {

                // Return the value if the response packet is valid
                return response_packet->value;
            }
        }

        // Increment the retry counter and try again
        retries++;
    }

    // Return -1 if the operation timed out
    return -1;
}

int RPC_put(struct rpc_connection *rpc, int key, int value) {
    struct packet_info packet;
    int retries = 0;

    // Create the request packet
    struct rpc_packet request_packet;
    request_packet.client_id = rpc->client_id;
    request_packet.seq_number = rpc->seq_number++;
    request_packet.opcode = RPC_OPCODE_PUT;
    request_packet.key = key;
    request_packet.value = value;

    while (retries < RETRY_COUNT) {
        // Send the request packet to the server
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, (char *)&request_packet, sizeof(struct rpc_packet));

        // Wait for a response packet
        packet = receive_packet_timeout(rpc->recv_socket, TIMEOUT_TIME);

        // Check if the response packet is valid
        if (packet.recv_len == sizeof(struct rpc_packet)) {
            struct rpc_packet *response_packet = (struct rpc_packet *)packet.buf;

            if (response_packet.client_id == rpc.client_id &&
                response_packet.seq_number == request_packet.seq_number &&
                response_packet.opcode == RPC_OPCODE_PUT_RESPONSE) {

                // Return 0 if the response packet is valid
                return 0;
            }
        }

        // Increment the retry counter and try again
        retries++;
    }

    // Return -1 if the operation timed out
    return -1;
}

void RPC_close(struct rpc_connection *rpc) {
    close_socket(rpc->recv_socket);
}