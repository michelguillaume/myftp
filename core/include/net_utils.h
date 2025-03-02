/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** No file there, just an epitech header example.
** You can even have multiple lines if you want!
*/

#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int socket_t;

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)

socket_t create_tcp_server(uint16_t port);
socket_t create_tcp_socket(void);
bool bind_socket(socket_t sockfd, const char *ip_address, uint16_t port);
bool listen_socket(socket_t sockfd, int backlog);
socket_t accept_tcp_connection(socket_t server_sock, struct sockaddr *client_addr, socklen_t *addr_len);
socket_t accept_tcp_connection_impl(socket_t sockfd, struct sockaddr *client_addr, socklen_t *addr_len);
int close_connection(socket_t sockfd);
bool close_socket(socket_t sockfd);


ssize_t send_data(socket_t sockfd, const char *buffer, int length);
ssize_t receive_data(socket_t sockfd, char *buffer, int length);

bool convert_ip_address(const char *ip_address, uint16_t port, struct sockaddr_in *addr);
bool convert_address_to_string(const struct sockaddr_in *addr, char *ip_str, size_t max_length);

void print_socket_error(const char *message);

#endif
