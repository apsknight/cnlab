/**
 * Computer Networks Lab: Assignment-1
 * Server Implementation for message sharing using sockets.
 * Author: Aman Pratap Singh
 */

// Include Headers
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <pthread.h>

#define PORT 8081
#define MAXLEN 50

void *chat(void*);

int main(int argc, char** argv) {
    int sock_fd, client_fd;
    socklen_t client_size;
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

    int yes = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Append zero to rest of the struct.
    bzero(&server.sin_zero, 8);

    // Bind socket to server
    if ( (bind(sock_fd, (struct sockaddr *)&server, sizeof(server))) != 0) {
        perror("Bind API Error!\n");
        exit(1);
    }
    else {
        fprintf(stderr, "Socket succesfully binded!\n");
    }

    // Listen for incoming connections
    if ( (listen(sock_fd, 5)) != 0) {
        perror("Listen API Error!\n");
        exit(1);
    }
    else {
        fprintf(stderr, "Socket succesfully listening!\n");
    }

    while(1) {
        client_size = sizeof(client);
        bzero(&client, client_size);
        client_fd = accept(sock_fd, (struct sockaddr *)&client, &client_size);

        if (client_fd == -1) {
            perror("Accept API Failed!\n");
            exit(1);
        }
        else {
            printf("Accepted a Client!\n");
        }

        pthread_t tid;
        pthread_create(&tid, NULL, chat, (void *)&client_fd);
        // chat(client_fd);
        // pthread_exit(NULL);
    }


    close(sock_fd);
}

// Function to chat with client
void* chat(void* sockfd) {
    int sock_fd = *((int*)sockfd);
    char msg[MAXLEN];
    int n;

    while(1) {
        bzero(msg, MAXLEN);
        read(sock_fd, msg, sizeof(msg));
        printf("From Client %d: %sTo Client %d: ", sock_fd, msg, sock_fd);

        n = 0;

        while( (msg[n++] = getchar()) != '\n' );

        write(sock_fd, msg, sizeof(msg));

        if (strcmp("exit", msg) == 0) {
            printf("Closing Connection..!\n");
            // return;
        }
    }
}