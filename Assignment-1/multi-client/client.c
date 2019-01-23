/**
 * Computer Networks Lab: Assignment-1
 * Client Implementation for message sharing using sockets.
 * Client is same as Single Client Program
 * Author: Aman Pratap Singh
 */

// Include Headers
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h> 

#include "utils.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main(int argc, char** argv) {
    // Set stdout as unbuffered.
    setvbuf(stdout, NULL, _IONBF, 0);

    // Detect port and IP from command line argument, if provided.
    int portnum = PORT;
    char *IP = SERVER_IP;
    if (argc >= 2) {
        portnum = atoi(argv[1]);
    }
    if (argc >= 3) {
        IP = argv[2];
    }
    printf("Pinging on PORT %d at %s\n", portnum, IP);

    // Create Socket and start listening on `portnum`.
    int sockt = connect_inet_socket(portnum, IP);
    if (sockt < 0) {
        perror_die("Connection Failed!");
    }
    else {
        printf("Connected to the client\n");
    }

    char sendline[BUFSIZ], recvline[BUFSIZ];
    while (fgets(sendline, BUFSIZ, stdin) != NULL) {
        send(sockt, sendline, strlen(sendline), 0);
                
        if (recv(sockt, recvline, BUFSIZ,0) == 0){
            perror("The server terminated prematurely"); 
            exit(1);
        }
        printf("String received from the server: ");
        fputs(recvline, stdout);
    }

    return 0;
}