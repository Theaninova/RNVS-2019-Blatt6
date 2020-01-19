#include <time.h>
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
#include "client.h"

// aus Vorgabe
byte64 Unix_timestamp = 2208988800;

DEBUGGABLE_MAIN(argc, argv)
    //STR_ARG(NTP_server_name, 0)
    //STR_ARG(count, 1)
    uint8_t port = 123;
    uint16_t  n = atoi(argv[1]);
    LOG_INT(n);

    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family      = AF_INET;
    hint.ai_socktype    = SOCK_DGRAM;
    hint.ai_protocol    = IPPROTO_UDP;
    struct sockaddr_in serv_addr;       // TODO to change to getaddressinfo-lookup

    t_Cache tsrv[argc-2];
    for(int i = 0; i<n; i++){
        for(int j = 0; j<(argc-2); j++) {
            //byte order conversion
            //tsrv[j].runnum = j;
            serv_addr.sin_port = htons(port);
            RawTimeserverProtocol *request = (RawTimeserverProtocol *) calloc(1, sizeof(RawTimeserverProtocol));
            request->header = (3u << 3u) | (3u);   //(3u << 5u) | (4u << 2u);    TODO: check Byte order
            LOG_INT(request->header);
            if ((tsrv[j].server = gethostbyname(argv[2 + j])) == NULL) LOG("ERROR, host not found.");   // TODO redundant resolving of the address...not very nice, but I'm really tired
            if ((tsrv[j].client_socket = socket(hint.ai_family, hint.ai_socktype, hint.ai_protocol)) < 0)
                LOG("ERROR, host not found.");
            serv_addr.sin_family = AF_INET;
            memcpy((byte8 *) &serv_addr.sin_addr.s_addr, (byte8 *) tsrv[j].server->h_addr, tsrv[j].server->h_length);
            if (connect(tsrv[j].client_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) LOG("ERROR, connection fail");
            if (send(tsrv[j].client_socket, (byte8 *) request, sizeof(RawTimeserverProtocol), 0) < 0) LOG("ERROR, data send error");
            if (recv(tsrv[j].client_socket, (byte8 *) request, sizeof(RawTimeserverProtocol), 0) < 0) LOG("ERROR, data read error");
            // to network byte order
            request->transmit_timestamp = ntohl(request->transmit_timestamp);
            time_t t_network = request->transmit_timestamp - Unix_timestamp;
            //LOG(ctime((time_t *) &t_network));
            free(request);
        }
        i++;
        sleep(1);
    }
    LOG("All done");
    return 0;
}


