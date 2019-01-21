#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#define PORT 8080

void* clientHandler(void* sockt_fd);
char* scrapFilename(char* request);
bool sendFiletoClient(int sockt_fd, char* file_name);
void ls(char *);

int main(int argc, char **argv) {
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
	
	while (1) {
        client_size = sizeof(client);
        bzero(&client, client_size);
        client_fd = accept(sock_fd, (struct sockaddr *)&client, &client_size);
        printf("Accepted a new client.\n");
		pthread_t client_thread;
		pthread_create(&client_thread, NULL,  clientHandler, (void*)&client_fd);
		pthread_join(client_thread, NULL);
	}

	return 0;
}

void ls(char* msg) {
    memset(msg, 0, BUFSIZ);
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 
                && strchr(ent->d_name, '.')) {
                strcat(msg, ent->d_name);
                strcat(msg, "\n");
            }
        }
        closedir (dir);
    }
    else {
        perror("Unable to access files.");
    }
}

void *clientHandler(void *sockt_fd) {
	int	socket = *(int*) sockt_fd;   
	char server_msg[BUFSIZ], client_msg[BUFSIZ], file_name[BUFSIZ];
    
    ls(server_msg);
    // Send available files to Client
    write(socket, server_msg, BUFSIZ);
    memset(server_msg, 0, BUFSIZ);
    // Read File asked by client.
	read(socket, client_msg, 100);
    printf("Client Asked: %s\n", client_msg);
	strcpy(file_name, scrapFilename(client_msg));

	if (access(file_name, F_OK) != -1) {
		strcpy(server_msg, "Okay");
		write(socket, server_msg, 100);
		sendFiletoClient(socket, file_name);
	}
	else {
		strcpy(server_msg, "Nope");
		write(socket, server_msg, strlen(server_msg)); 
	}

	return 0;
}

char* scrapFilename(char* request) {

	// char *file_name = strchr(request, ' ');
	// return file_name + 1;
    return request;
}

bool sendFiletoClient(int sockt_fd, char* file_name) {

	struct stat	file_info;
	int	file_fd, file_size;

	stat(file_name, &file_info);
	file_fd = open(file_name, O_RDONLY);
	file_size = file_info.st_size;
	write(sockt_fd, file_name, 100);
	
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL) {
        printf("File open error!\n");
        exit(1);
    }

    while(1) {
        char buff[BUFSIZ] = {0};
        int nread = fread(buff, 1, 1024, fp);

        if (nread > 0) {
            write(sockt_fd, buff, nread);
        }
        if (nread < 1024) {
            if (feof(fp)) {
                printf("Send complete!\n");
                break;
            }
            if (ferror(fp)) {
                printf("Error Reading File!\n");
                break;
            }
        }
    }


	return true;
}