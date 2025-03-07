/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_noop.c
*/

#include "myftp.h"
#include "cvector.h"

void my_noop(server_t *, char *, peer_t *conn)
{
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "200\r\n", 5) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}
