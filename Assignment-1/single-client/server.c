/**
 * Computer Networks Lab: Assignment-1
 * Server Implementation for message sharing using sockets.
 * Author: Aman Pratap Singh
 */

// Include Headers
#include <stdlib.h> 
#include <stdio.h>
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>

#include "utils.h"

#define PORT 8080

int main(int argc, char** argv) {
    // Set stdout as unbuffered.
    setvbuf(stdout, NULL, _IONBF, 0);

    // Detect port from command line argument, if provided.
    int portnum = PORT;
    if (argc >= 2) {
        portnum = atoi(argv[1]);
    }
    printf("Serving on PORT %d\n", portnum);

    // Create Socket and start listening on `portnum`.
    int sockt = listen_inet_socket(portnum);

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);

    int peer_sock_fd = accept(sockt, (struct sockaddr *)&peer_addr, &peer_addr_len);

    if (peer_sock_fd < 0) {
        printf("Error while accepting connection\n");
    }

    // Report connection with peer.
    report_peer_connected(&peer_addr, peer_addr_len);

    // Handle Chat with Peer
    handle_chat(peer_sock_fd);

    close(sockt);
}