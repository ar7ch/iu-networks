#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "lab6.h"

#define BUF_SIZE 80
#define PORT 8082


// =========================
char endnode_adjacent_ip[] = "192.168.31.245";

char midnode_before_ip[]   = "192.163.3.30";
char midnode_after_ip[]    = "192.168.0.1";


char hello_message[] = "hello";
// =========================

struct sockaddr_in this_host;
int this_host_socket;


void init_socket() {
    this_host_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this_host_socket < 0) {
        die("socket");
    }
}

void init_bind() {
    bzero(&this_host, sizeof(this_host));
    this_host.sin_family = AF_INET;
    this_host.sin_port = htons(PORT);
    this_host.sin_addr.s_addr = htons(INADDR_ANY);
    if(bind(this_host_socket, (struct sockaddr *) &this_host, sizeof(this_host)) < 0)
        die("bind");
}

void init() {
    init_socket();
    init_bind();
}

struct sockaddr_in get_sockaddr(char * ip) {
    struct sockaddr_in sa;
    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    if(inet_pton(AF_INET, ip, &(sa.sin_addr)) <= 0)
    {
        die("\nInvalid address/ Address not supported \n");
    }
    return sa;
}

void send_msg(char * dest_ip, char * msg) {
    struct sockaddr_in dest_addr = get_sockaddr(dest_ip);
    int out_socket = socket(AF_INET, SOCK_STREAM, 0);
    puts("Trying to connect...");
    if(connect(out_socket, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0)
        die("send_msg connect");
    puts("Send connect OK");

    if(send(out_socket, msg, strlen(msg), 0) <= 0)
        die("send_msg send");
    puts("Send OK");

    close(out_socket);
}

void receive_msg(char * src_ip, char * buf, size_t buf_size) {
    struct sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(src_addr);
    bzero(&src_addr, sizeof(src_addr));
    puts("Listening...");
    if(listen(this_host_socket, 2)) {
        die("listen");
    }
    int connection_socket;
    connection_socket = accept(this_host_socket, NULL, NULL );
    if(connection_socket< 0)
        die("accept");
    puts("Recv connect OK");
    if(recv(connection_socket, (void *)buf, buf_size, 0) <= 0) {
        die("recv receive hello");
    }
    puts("Recv OK");

    close(connection_socket);

    return;
}

int main(int argc, char ** argv) {
    init();
    if(argc != 2) {
        fail:
            printf("usage: %s {leftendnode, rightendnode, midnode}\n", argv[0]);
            return EXIT_FAILURE;
    }
    if(!strcmp(argv[1], "leftendnode")) {
        puts("I am left node");
        char buf[BUF_SIZE] = {0};
        send_msg(endnode_adjacent_ip, "Hello from the left node");
        receive_msg(endnode_adjacent_ip, buf, BUF_SIZE);
        printf("Recieved %s\n", buf);
    }
    else if(!strcmp(argv[1], "rightendnode")) {
        puts("I am right node");
        char buf[BUF_SIZE] = {0};
        receive_msg(endnode_adjacent_ip, buf, BUF_SIZE);
        send_msg(endnode_adjacent_ip, "Hello from the right node");
        printf("Recieved %s\n", buf);
    }
    else if(!strcmp(argv[1], "midnode")) {
        puts("I am midnode");
        char buf1[BUF_SIZE] = {0};
        char buf2[BUF_SIZE] = {0};
        receive_msg(midnode_before_ip, buf1, BUF_SIZE);
        send_msg(midnode_after_ip, buf1);
        printf("Retransmitted %s\n", buf1);
        receive_msg(midnode_after_ip, buf2, BUF_SIZE);
        send_msg(midnode_before_ip, buf2);
        printf("Retransmitted %s\n", buf2);
    }
    else {
        goto fail;
    }
    return 0;
}