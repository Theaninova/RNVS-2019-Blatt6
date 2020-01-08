/* T25 G04 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "helper/wulkanat/debug.h"
#include "helper/wulkanat/commander.h"
#include "helper/wulkanat/descriptive_types.h"

typedef struct __attribute__((packed)) {
    byte32 header;
    byte32 root_delay;
    byte32 root_dispersion;
    byte32 reference_id;
    byte64 reference_timestamp;
    byte64 origin_timestamp;
    byte64 receive_timestamp;
    byte64 transmit_timestamp;
} RawTimeserverProtocol;

DEBUGGABLE_MAIN(argc, argv)
    STR_ARG(address, 0)
    STR_ARG(port, 1)

	struct addrinfo *server_addr;
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	int32 err = getaddrinfo(address, port, &hint, &server_addr);
	if (err != false) {
		ERROR("Error while getting server address info");
		exit(1);
	}

	int32 client;

	/* Iterate over the provided addresses and try to establish a connection */
	for (struct addrinfo *cur_info = server_addr; cur_info != NULL; cur_info = cur_info->ai_next) {	
		client = socket(cur_info->ai_family, cur_info->ai_socktype, cur_info->ai_protocol);

		err = connect(client, cur_info->ai_addr, sizeof(*(cur_info->ai_addr)));
		if (err == 0) break;
		if (err == -1) {
			ERROR("Error while connecting");
			close(client);
			exit(1);
		}
		if (cur_info->ai_next == NULL) {
			ERROR("Error could not connect to any service");
			exit(1);
		}

		close(client);
	}
	freeaddrinfo(server_addr);

	RawTimeserverProtocol buffer;
	int32 bytes_received = 0;

	while ((bytes_received = recv(client, &buffer, sizeof(buffer), 0)) != 0) {
		if (bytes_received == -1) {
			ERROR("Error while receiving message from server");
			break;
		}
		HEX_VALUE_LOG(&buffer, bytes_received)
	}

	close(client);
}