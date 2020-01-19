#ifndef BLATT_06_CLIENT_H
#define BLATT_06_CLIENT_H

typedef struct  {
    time_t tv_sec; /* seconds */
    long tv_nsec; /* nanoseconds */
}timespec;

struct t_servers{
    int32               client_socket;
    byte16              servernum;
    byte16              runnum;
    time_t              time;
    timespec            tstmp_send;
    timespec            tstmp_rec;
    struct hostent*     server;
};

typedef struct t_servers t_Cache;

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

#endif
