/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** convert_address_to_string.c
*/

#include "net_utils.h"

bool convert_address_to_string(const struct sockaddr_in *addr, char *ip_str, size_t max_length)
{
    if (max_length < INET_ADDRSTRLEN) {
        fprintf(stderr, "Buffer too small for IP address\n");
        return false;
    }
    if (inet_ntop(AF_INET, &addr->sin_addr, ip_str, max_length) == NULL) {
        print_socket_error("inet_ntop failed");
        return false;
    }
    return true;
}
