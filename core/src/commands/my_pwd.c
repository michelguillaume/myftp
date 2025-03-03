/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_pwd.c
*/

#include "myftp.h"
#include "cvector.h"

static int test_arg_pwd(char *arg, peer_t *conn)
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

void my_pwd(server_t *srv, char *arg, peer_t *conn)
{
    if (conn->user_data.state == AUTH) {
        if (test_arg_pwd(arg, conn) == FAILURE)
            return;
        if (vector_push_back(conn->sending_buffer, "257 ", 4)
                == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                            "to sending_buffer\n");
        if (vector_push_back(conn->sending_buffer, conn->user_data.pwd,
                strlen(conn->user_data.pwd)) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                            "to sending_buffer\n");
        if (vector_push_back(conn->sending_buffer, " is"
            " on directory\n\r", 18) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                            "to sending_buffer\n");
    } else {
        if (vector_push_back(conn->sending_buffer, "530 Not logged in.\r\n", 20) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
    }
}