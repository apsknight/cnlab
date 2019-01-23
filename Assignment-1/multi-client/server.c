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
#include <pthread.h>

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
    int sock_fd = listen_inet_socket(portnum);

    // Start Accepting clients
    while(1) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        char buf[BUFSIZ];
        int n = 0;

        // accept a connection
        int connfd = accept(sock_fd, (struct sockaddr *) &peer_addr, &peer_addr_len);
        report_peer_connected(&peer_addr, peer_addr_len);


        printf("%s\n","Received request...");   

        pid_t childpid;
        if ( (childpid = fork ()) == 0 ) {
            //if it’s 0, it’s child process
            printf ("%s\n","Child created for dealing with client requests");

            //close listening socket
            close(sock_fd);

            while ( (n = recv(connfd, buf, BUFSIZ, 0)) > 0)  {
                printf("String received from and resent to the client:");
                puts(buf);
                send(connfd, buf, n, 0);
            }

            if (n < 0) printf("%s\n", "Read error");

            exit(0);
        }

        //close socket of the server
        close(connfd);
    }

    return 0;
}