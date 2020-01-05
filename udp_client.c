#include <linux/input.h>
#include "udp_client.h"

void init_header(client *c) {
    c->header.reference = 1;
}

void init_socket_fd(client *c)
{
    if ((c->socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        exit(EXIT_FAILURE); 
    }
}

void init_server_addr(client *c, int server_port, char *server_addr)
{
    struct sockaddr_in *server_addr_in = (struct sockaddr_in *) calloc(1, sizeof(struct sockaddr_in)); 
      
    server_addr_in->sin_family = AF_INET; 
    server_addr_in->sin_port = htons(server_port); 
    if (inet_aton(server_addr , &server_addr_in->sin_addr) == 0) {
		exit(EXIT_FAILURE);
	}
    c->server_addr = (struct sockaddr *)server_addr_in;
    c->slen = sizeof(*server_addr_in);
}

client *init_client(int server_port, char *server_addr)
{
    client *c = (client *) malloc(sizeof(client));
    init_header(c);
    init_socket_fd(c);
    init_server_addr(c, server_port, server_addr);
    return c;
}

void close_client(client *c)
{
    close(c->socket_fd);
    free((struct sockaddr_in *)(c->server_addr));
    free(c);
}

int header_to_string(packet_header h, char *header_string) {

    const char *header_formatter = "%d";

    int n = sprintf(header_string, header_formatter,
                h.reference
            );
    if (n < 0) {
        return -1;
    }
    return strlen(header_string);
}

int build_packet(struct input_event ev, char *packet)
{
    const char *packet_formatter =
                            "%ld:"
                            "%x:\n";
    int n;
    n = sprintf(packet, packet_formatter,
                ev.time.tv_sec,
                ev.code
            );
    if (n < 0) {
        return -1;
    }
    return strlen(packet);
}

int send_packet(client *c, void *packet)
{
    int header_len;
    char header_string[256];
    header_len = header_to_string(c->header, header_string);

    char full_payload[1024];
    sprintf(full_payload, "%s::%s", header_string, (char *)packet);

    return sendto(c->socket_fd, full_payload,
                  strlen(full_payload),
                  0,
                  c->server_addr,
                  c->slen);
}
