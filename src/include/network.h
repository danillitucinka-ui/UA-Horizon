#ifndef NETWORK_H
#define NETWORK_H

#include "stdint.h"

#define NET_MAX_PACKETS 16
#define NET_MTU 1500

typedef struct {
    uint8_t data[NET_MTU];
    uint32_t size;
    uint32_t timestamp;
} net_packet_t;

// Network functions
void net_init(void);
void net_set_ip(uint8_t ip[4]);
void net_set_mac(uint8_t mac[6]);
int net_send_packet(uint8_t *data, uint32_t size);
int net_receive_packet(uint8_t *buffer, uint32_t *size);

// DNS functions
int dns_resolve(const char *hostname, uint8_t ip[4]);

// HTTP functions
int http_get(const char *url, char *response, uint32_t max_size);

// Download functions
int download_file(const char *url, const char *filename);

// TCP functions (simplified)
int tcp_connect(const char *hostname, uint16_t port);
int tcp_listen(uint16_t port);
int tcp_accept(int socket);
int tcp_read(int socket, char *buffer, uint32_t size);
int tcp_write(int socket, const char *data, uint32_t size);
void tcp_close(int socket);

// EXE execution
int exe_execute(const char *filename);

#endif