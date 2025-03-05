/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_cdup.c
*/

#include "myftp.h"
#include "cvector.h"

static int test_arg_cdup(char *arg, peer_t *conn)
{
    if (arg != NULL) {
        if (vector_push_back(conn->sending_buffer, "501 Syntax error in"
            " parameters or arguments.\r\n", 46) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                " to sending_buffer\n");
        return FAILURE;
    }
    return SUCCESS;
}

void my_cdup(server_t *srv, char *arg, peer_t *conn)
{
    if (conn->user_data.state == AUTH) {
        if (test_arg_cdup(arg, conn) == FAILURE)
            return;
        if (vector_push_back(conn->sending_buffer, "200 Command okay.\r\n", 19)
                == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                            "to sending_buffer\n");
        memset(conn->user_data.pwd, 0, sizeof(conn->user_data.pwd));
    } else {
        if (vector_push_back(conn->sending_buffer, "530 Not logged in.\r\n", 20) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
    }
}