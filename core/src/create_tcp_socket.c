/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** create_tcp_socket.c
*/

#include "net_utils.h"

socket_t create_tcp_socket(void)
{
    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == INVALID_SOCKET) {
        print_socket_error("socket failed");
        return INVALID_SOCKET;
    }
    return sockfd;
}
