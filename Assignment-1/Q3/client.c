#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void getFile(int sock_fd) {
    char filename[BUFSIZ], reply_msg[BUFSIZ];
    int file_size, file_desc;
    char* data;

    // Read available file at server
    read(sock_fd, reply_msg, BUFSIZ);
    printf("\nAvailable files at Server:\n");
    printf("\n***********\n");
    printf("%s", reply_msg);
    printf("***********\n\n");
    memset(reply_msg, 0, BUFSIZ);
    printf("Required File: ");
    scanf("%s", filename);
    printf("Trying to get file: %s\n", filename);

	write(sock_fd, filename, strlen(filename));
	read(sock_fd, reply_msg, BUFSIZ);
    printf("Recieved Msg: %s\n", reply_msg);

	if (strcmp(reply_msg, "Okay") == 0) {
        memset(filename, 0, BUFSIZ);
        read(sock_fd, &filename, BUFSIZ);
        printf("Creating File: %s\n", filename);
        FILE* fp = fopen(filename, "ab");
        if (fp == NULL) {
            printf("Error Reading File!\n");
            exit(1);
        }

        ssize_t recievedSize = 0;
        ssize_t byteCount = 0;
        while(1) {
            memset(&reply_msg, 0, BUFSIZ);
            recievedSize = read(sock_fd, reply_msg, 1024);
            byteCount += recievedSize;
            printf("\rRecieçved %zd KB", byteCount);
            fflush(stdout);
            if (fwrite(reply_msg, 1, recievedSize, fp) != recievedSize) {
                printf("Writing error!!\n");
                return;
            }
            if (recievedSize < 1024) break;
        }
        if (recievedSize < 0) {
            printf("Download Error\n");
        }

        printf("\nTask Completed..!\n");
    }
	else {
		fprintf(stderr, "Bad request\n");
	}
}

int main(int argc , char **argv) {
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

	// Get a file from server
    getFile(sock_fd);
	

	return 0;
}