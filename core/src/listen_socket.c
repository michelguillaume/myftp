/*
** EPITECH PROJECT, 2023
** myftp
** File description:
** listen_socket.c
*/

#include "net_utils.h"

bool listen_socket(socket_t sockfd, int backlog)
{
    if (listen(sockfd, backlog) == SOCKET_ERROR) {
        print_socket_error("listen failed");
        return false;
    }
    return true;
}
