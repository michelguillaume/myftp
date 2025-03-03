/*
** EPITECH PROJECT, 2023
** myftp
** File description:
** mynetworks.c
*/

#include "net_utils.h"

socket_t create_tcp_server(uint16_t port)
{
    socket_t server_sock = create_tcp_socket();

    if (server_sock == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }
    if (!bind_socket(server_sock, "0.0.0.0", port)) {
        close_connection(server_sock);
        return INVALID_SOCKET;
    }
    if (!listen_socket(server_sock, 5)) {
        close_connection(server_sock);
        return INVALID_SOCKET;
    }
    return server_sock;
}

socket_t accept_tcp_connection(socket_t server_sock,
    struct sockaddr *client_addr, socklen_t *addr_len)
{
    char client_ip[INET_ADDRSTRLEN] = {0};
    socket_t client_sock = accept_tcp_connection_impl(server_sock, client_addr, addr_len);

    if (client_sock == INVALID_SOCKET) {
        return client_sock;
    }
    if (convert_address_to_string((struct sockaddr_in *) client_addr, client_ip, INET_ADDRSTRLEN)) {
        printf("Connection accepted from %s\n", client_ip);
    }
    return client_sock;
}

ssize_t send_data(socket_t sockfd, const char *buffer, int length)
{
    ssize_t bytes_sent = write(sockfd, buffer, length);

    if (bytes_sent == -1) {
        print_socket_error("write failed");
    }
    return bytes_sent;
}

ssize_t receive_data(socket_t sockfd, char *buffer, int length)
{
    ssize_t bytes_received = read(sockfd, buffer, length);

    if (bytes_received < 0) {
        print_socket_error("read failed");
    }
    return bytes_received;
}

int close_connection(socket_t sockfd)
{
    if (shutdown(sockfd, SHUT_RDWR) == SOCKET_ERROR) {
        print_socket_error("close_connection: shutdown failed");
        return false;
    }
    if (close(sockfd) == SOCKET_ERROR) {
        print_socket_error("close_connection: close failed");
        return false;
    }
    return true;
}

bool abort_connection(socket_t sockfd)
{
    if (close(sockfd) == SOCKET_ERROR) {
        print_socket_error("abort_connection: close failed");
        return false;
    }
    return true;
}
