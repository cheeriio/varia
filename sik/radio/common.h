#ifndef MIMUW_SIK_TCP_SOCKETS_COMMON_H
#define MIMUW_SIK_TCP_SOCKETS_COMMON_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "err.h"

inline static void check_correct_number_string(char* string){
    for(size_t i = 0; i < strlen(string); i++){
        if(!isdigit(string[i])){
            fatal("%s is not a positive number", string);
        }
    }
}

inline static uint32_t read_unsigned_number(char* string){
    check_correct_number_string(string);
    uint32_t ret = atoi(string);
    return ret;
}

inline static uint16_t read_port(char *string) {
    errno = 0;
    check_correct_number_string(string);
    unsigned long port = strtoul(string, NULL, 10);
    PRINT_ERRNO();
    if (port > UINT16_MAX) {
        fatal("%u is not a valid port number", port);
    }

    return (uint16_t) port;
}

inline static void set_port_reuse(int socket_fd) {
    int option_value = 1;
    CHECK_ERRNO(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &option_value, sizeof(option_value)));
}


struct sockaddr_in get_send_address(char *host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo *address_result;
    CHECK(getaddrinfo(host, NULL, &hints, &address_result));

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET; // IPv4
    send_address.sin_addr.s_addr =
            ((struct sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr; // IP address
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}

int bind_socket(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
    set_port_reuse(socket_fd);
    ENSURE(socket_fd >= 0);
    // after socket() call; we should close(sock) on any execution path;

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // listening on all interfaces
    server_address.sin_port = htons(port);

    // bind the socket to a concrete address
    CHECK_ERRNO(bind(socket_fd, (struct sockaddr *) &server_address,
                        (socklen_t) sizeof(server_address)));

    return socket_fd;
}

inline static void start_listening(int socket_fd, size_t queue_length) {
    CHECK_ERRNO(listen(socket_fd, queue_length));
}

inline static uint16_t get_port(struct sockaddr_in *address) {
    return ntohs(address->sin_port);
}

/// No need to free the returned string,
/// it is a pointer to a static buffer
inline static char *get_ip(struct sockaddr_in *address) {
    return inet_ntoa(address->sin_addr);
}

inline static void connect_socket(int socket_fd, const struct sockaddr_in *address) {
    CHECK_ERRNO(connect(socket_fd, (struct sockaddr *) address, sizeof(*address)));
}

inline static void send_message(int socket_fd, const void *message, size_t length, int flags) {
    errno = 0;
    ssize_t sent_length = send(socket_fd, message, length, flags);
    if (sent_length < 0) {
        PRINT_ERRNO();
    }
    ENSURE(sent_length == (ssize_t) length);
}

inline static size_t receive_message(int socket_fd, void *buffer, size_t max_length, int flags) {
    errno = 0;
    ssize_t received_length = recv(socket_fd, buffer, max_length, flags);
    if (received_length < 0) {
        PRINT_ERRNO();
    }
    return (size_t) received_length;
}

inline static int accept_connection(int socket_fd, struct sockaddr_in *client_address) {
    socklen_t client_address_length = (socklen_t) sizeof(*client_address);

    int client_fd = accept(socket_fd, (struct sockaddr *) client_address, &client_address_length);
    if (client_fd < 0) {
        PRINT_ERRNO();
    }

    return client_fd;
}

#endif //MIMUW_SIK_TCP_SOCKETS_COMMON_H