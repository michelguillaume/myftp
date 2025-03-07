/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_user.c
*/

#include "myftp.h"
#include "cvector.h"

void my_user(server_t *, char *arg, peer_t *conn)
{
    if (!arg) {
        if (VECTOR_PUSH_BACK(conn->sending_buffer,
            "500 Syntax error, command unrecognized.\r\n", 41)
            == VECTOR_FAILURE)
            fprintf(stderr,
                "Error: Failed to push error message to sending_buffer\n");
        return;
    }
    strncpy(conn->user_data.username,
        arg, USERNAME_SIZE - 1);
    conn->user_data.state = WAIT_PASS;
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "331 User name okay, need password.\r\n", 36) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}
