#include "utils.h"

#include <pthread.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <netdb.h>

#define N_BACKLOG 64

void die(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

void *xmalloc(size_t size) {
	void *ptr = malloc(size);
	if (!ptr) {
		die("Malloc Failed!");
	}
	return ptr;
}

void perror_die(char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void report_peer_connected(const struct sockaddr_in *sa, socklen_t salen) {
	char hostbuf[NI_MAXHOST];
	char portbuf[NI_MAXSERV];
	if (getnameinfo((struct sockaddr *)sa, salen, hostbuf, NI_MAXHOST, portbuf,
					NI_MAXSERV, 0) == 0) {
		printf("\nPeer (%s, %s) connected\n", hostbuf, portbuf);
	} else {
		printf("Peer (unknonwn) connected\n");
	}
}

int listen_inet_socket(int portnum) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror_die("ERROR opening socket");
	}

	// This helps avoid spurious EADDRINUSE when the previous instance of this
	// server died.
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror_die("setsockopt");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portnum);
	// Append zero to rest of the struct.
    bzero(&serv_addr.sin_zero, 8);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror_die("ERROR on binding");
	}

	if (listen(sockfd, N_BACKLOG) < 0) {
		perror_die("ERROR on listen");
	}

	return sockfd;
}

int connect_inet_socket(int portnum, char* IP) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror_die("ERROR opening socket");
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(IP);
	serv_addr.sin_port = htons(portnum);
	// Append zero to rest of the struct.
    bzero(&serv_addr.sin_zero, 8);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
		return sockfd;
	}

	return -1;
}

void make_socket_non_blocking(int sockfd) {
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror_die("fcntl F_GETFL");
	}

	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror_die("fcntl F_SETFL O_NONBLOCK");
	}
}

void* readHandler(void* sockt) {
    int	sock_peer = *(int*) sockt;
    char response[BUFSIZ];
    
    while(1) {
        memset(response, 0, BUFSIZ);
        int len = recv(sock_peer, response, BUFSIZ, 0);
		if (len < 0) {
			perror_die("recv");
		}
		else if (len == 0) {
			// Connection closed by peer.
			return 0;
		}
	
        printf("\nPeer: %s\n", response);
        if (strcmp(response, "Bye") == 0) {
            return 0;
        }
    }

    return 0;
}

void* writeHandler(void* sockt) {
    int	sock_peer = *(int*) sockt;
    char request[BUFSIZ];
    
    while(1) {
        memset(request, 0, BUFSIZ);
        int n = 0;
        while( (request[n++] = getchar()) != '\n');

        send(sock_peer, request, BUFSIZ, 0);

        if (strcmp(request, "Bye") == 0) {
            return 0;
        }
    }

    return 0;
}

// Function to chat with client
void handle_chat(int sockt) {
    char request[BUFSIZ], response[BUFSIZ];
    pthread_t read_thread, write_thread;

    // Create threads for chatting.
    pthread_create(&read_thread, NULL,  readHandler, (void*)&sockt);
    pthread_create(&write_thread, NULL,  writeHandler, (void*)&sockt);

    // Join threads with main thread.
    pthread_join(read_thread, NULL);
    pthread_join(write_thread, NULL);

    close(sockt);
    return;
}