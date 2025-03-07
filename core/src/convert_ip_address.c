/*
** EPITECH PROJECT, 2023
** myftp
** File description:
** convert_ip_address.c
*/

#include "net_utils.h"

bool convert_ip_address(const char *ip_address,
    uint16_t port, struct sockaddr_in *addr)
{
    if (inet_pton(AF_INET, ip_address, &(addr->sin_addr)) != 1) {
        print_socket_error("inet_pton failed");
        return false;
    }
    addr->sin_port = htons(port);
    return true;
}
