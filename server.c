#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

const char *emerg_quote = "If you dont have a quote of the day\nwrite your own!\n\t-- The programmer of this\n";

int main (int argc, char ** argv) {
	if (argc < 2) {
		printf("Error too few arguments\nUsage: server <port> <textfile>\n");
		exit(1);
	}

	/* seed the prng for a random quote */
	srand(time(NULL));

	FILE *quotes = fopen(argv[2], "r");
	if (quotes == NULL) {
		perror("Error while opening file");
		exit(1);
	}

	struct addrinfo *server_info;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err = getaddrinfo(NULL, argv[1], &hints, &server_info);
	if (err != 0) {
		perror("Error while getting server address");
		exit(1);
	}

	int server = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	err = bind(server, (struct sockaddr *) (server_info->ai_addr), sizeof(*(server_info->ai_addr)));
	freeaddrinfo(server_info);

	if (err != 0) {
		perror("Error while binding socket");
		close(server);
		exit(1);
	}

	err = listen(server, 1);
	if (err != 0) {
		perror("Error while listening to socket");
		close(server);
		exit(1);
	}

	struct sockaddr client_addr;
	socklen_t client_addrlen;
	int client;

	client = accept(server, &client_addr, &client_addrlen);
	if (client == -1) {
		perror("Error while accepting incoming connection");
		close(server);
		exit(1);
	}

	char *buffer = NULL;
	size_t len = 0;
	int bytes_read = getline(&buffer, &len, quotes);

	/* If the file is empty send the emergency quote. Else send a random quote from the provided file */
	if (bytes_read < 1) {
		send(client, emerg_quote, strlen(emerg_quote), 0);
	} else {
		char ch;
		int num_lines = 0;
		/* Count the number of lines */
		while ((ch = fgetc(quotes)) != EOF) {
			if (ch == '\n') num_lines++;
		}
		rewind(quotes);

		/* Get a random line */
		int rnd_line = rand() % (num_lines - 2);
		for (int i = 0; i < rnd_line; i++) {
			free(buffer);
			len = 0;
			getline(&buffer, &len, quotes);
		}

		err = send(client, buffer, bytes_read - 1, 0);
		if (err == -1) perror("Error while sending quote");
	}

	free(buffer);
	fclose(quotes);
	close(client);
	close(server);
}