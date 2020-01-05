#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

typedef struct packet_header {
    int reference;
} packet_header;

typedef struct client {
    int socket_fd;
    struct sockaddr *server_addr;
    socklen_t slen;
    packet_header header;
} client;

client *init_client(int server_port, char *server_addr);

void close_client(client *c);

int build_packet(struct input_event ev, char *packet);

int send_packet(client *c, void *packet);
