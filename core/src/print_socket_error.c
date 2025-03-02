/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** print_socket_error.c
*/

#include "net_utils.h"

void print_socket_error(const char *message)
{
    fprintf(stdout, "%s: %s\n", message, strerror(errno));
}
