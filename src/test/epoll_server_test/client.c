#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int socket_fd;
	
	
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	if(3 != argc) {
		fprintf(stderr, "Usage: %s <server_ip> <server port>", argv[0]);
		exit(1);
	}

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);	
	if(socket_fd < 0) {
		fprintf(stderr, "create socket error!");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	len = sizeof(server_addr);

	if(connect(socket_fd, (struct sockaddr *)&server_addr, len))	


	return 0;
}


