/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** close_socket.c
*/

#include "net_utils.h"

bool close_socket(socket_t sockfd)
{
    if (shutdown(sockfd, SHUT_RDWR) == SOCKET_ERROR) {
        print_socket_error("shutdown failed");
        return false;
    }
    if (close(sockfd) == SOCKET_ERROR) {
        print_socket_error("close failed");
        return false;
    }
    return true;
}
