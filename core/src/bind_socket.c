/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** bind_socket.c
*/

#include "net_utils.h"

bool bind_socket(socket_t sockfd, const char *ip_address, uint16_t port)
{
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    if (!convert_ip_address(ip_address, port, &addr)) {
        return false;
    }
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        print_socket_error("bind failed");
        return false;
    }
    return true;
}
