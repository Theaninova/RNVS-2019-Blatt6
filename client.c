/* T25 G04 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main (int argc, char ** argv) {
	if (argc < 3) {
		printf("Error to few arguments.\nUsage: client <ip/host name> <port>\n");
		exit(1);
	}

	struct addrinfo *server_addr;
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	int err = getaddrinfo(argv[1], argv[2], &hint, &server_addr);
	if (err != 0) {
		perror("Error while getting server address info");
		exit(1);
	}

	int client;

	/* Iterate over the provided addresses and try to establish a connection */
	for (struct addrinfo *cur_info = server_addr; cur_info != NULL; cur_info = cur_info->ai_next) {	
		client = socket(cur_info->ai_family, cur_info->ai_socktype, cur_info->ai_protocol);

		err = connect(client, cur_info->ai_addr, sizeof(*(cur_info->ai_addr)));
		if (err == 0) break;
		if (err == -1) {
			perror("Error while connecting");
			close(client);
			exit(1);
		}
		if (cur_info->ai_next == NULL) {
			printf("Error could not connect to any service\n");
			exit(1);
		}

		close(client);
	}
	freeaddrinfo(server_addr);

	char buffer[512];
	int bytes_received = 0;

	while ((bytes_received = recv(client, buffer, sizeof(buffer), 0)) != 0) {
		if (bytes_received == -1) {
			perror("Error while receiving message from server\n");
			break;
		}
		fwrite(buffer, bytes_received, 1, stdout);
	}

	close(client);
}