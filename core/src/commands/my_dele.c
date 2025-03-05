/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_dele.c
*/

#include "myftp.h"
#include "cvector.h"


static int test_arg_dele(char *arg, peer_t *conn)
{
    if (arg == NULL) {
        if (vector_push_back(conn->sending_buffer, "501 Syntax error in"
            " parameters or arguments.\r\n", 46) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                " to sending_buffer\n");
        return FAILURE;
    }
    return SUCCESS;
}

void my_dele(server_t *srv, char *arg, peer_t *conn) {
    char new_path[PATH_MAX];
    char *tmp;
    if (conn->user_data.state == AUTH) {
        if (test_arg_dele(arg, conn) == FAILURE)
            return;
        snprintf(new_path, PATH_MAX, "%s%s/%s",srv->path, conn->user_data.pwd, arg);
        tmp = realpath(new_path, nullptr);
        if (tmp == NULL) {
            if (vector_push_back(conn->sending_buffer, "501 Syntax"
                " error in parameters or arguments.\r\n", 46) == VECTOR_FAILURE)
                fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
            return;
        }
        remove(tmp);
        if (vector_push_back(conn->sending_buffer, "250 Requested file action okay, completed.\r\n", 44)
               == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message"
                            "to sending_buffer\n");
    } else {
        if (vector_push_back(conn->sending_buffer, "530 Not logged in.\r\n", 20) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
    }
}