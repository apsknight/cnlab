/**
 * Computer Networks Lab: Assignment-1
 * Server Implementation for message sharing using sockets.
 * Author: Aman Pratap Singh
 */

// Include Headers
#include <netdb.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#define PORT 8081
#define MAXLEN 50

void chat(int sock_fd);

int main(int argc, char** argv) {
    int sock_fd, server_fd;
    struct sockaddr_in server, client;

    // Create Socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock_fd == -1) {
        perror("Socket API Error!\n");
        exit(1);
    }
    else {
        fprintf(stderr, "Socket succesfully created!\n");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    // Append zero to rest of the struct.
    bzero(&(server.sin_zero), 8);

    // Connect
    server_fd = connect(sock_fd, (struct sockaddr *)&server, sizeof(server));

    if (server_fd != 0) {
        perror("Client API Failed!\n");
        exit(1);
    }
    else {
        printf("Connected to the Server!\n");
    }

    chat(sock_fd);

    close(sock_fd);
}

// Function to chat with client
void chat(int sock_fd) {
    char msg[MAXLEN];
    int n;

    while(1) {
        bzero(msg, sizeof(msg));
        printf("To Server: ");
        n = 0;
        while( (msg[n++] = getchar()) != '\n');
        write(sock_fd, msg, sizeof(msg));
        bzero(msg, sizeof(msg));
        read(sock_fd, msg, sizeof(msg));
        printf("From Server: %s", msg);
    }
}