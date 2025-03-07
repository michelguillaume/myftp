/*
** EPITECH PROJECT, 2023
** myftp
** File description:
** mynetworks_bis.c
*/

#include "net_utils.h"

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
