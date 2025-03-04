/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** tcp_accept_connection_impl.c
*/

#include "net_utils.h"

socket_t accept_tcp_connection_impl(socket_t sockfd,
    struct sockaddr *client_addr, socklen_t *addr_len)
{
    const socket_t client_sock = accept(sockfd, client_addr, addr_len);

    if (client_sock == INVALID_SOCKET) {
        print_socket_error("accept failed");
        return INVALID_SOCKET;
    }
    return client_sock;
}
