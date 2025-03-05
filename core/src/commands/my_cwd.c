/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** command.c
*/

#include "myftp.h"
#include "cvector.h"

/*
250
500, 501, 502, 421, 530, 550
500 Syntax error, command unrecognized. This may include errors such as command line too long. *
501 Syntax error in parameters or arguments.
502 Command not implemented. ??
421 Service not available, closing control connection. This may be a reply to any command if the service knows it must shut down.
530 Not logged in.
550 Requested action not taken. File unavailable (e.g., file not found, no access).
*/
void my_cwd(server_t *srv, char *arg, peer_t *conn) {
    char new_path[PATH_MAX];
    char *tmp;
    char *new_arg[PATH_MAX];

    if (conn->user_data.state == NOT_AUTH)
    {
        if (vector_push_back(conn->sending_buffer, "530 Not logged in.\r\n", 20) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
        return;
    }
    if (!arg) {
       if (vector_push_back(conn->sending_buffer,
           "500 Syntax error, command unrecognized.\r\n", 41) == VECTOR_FAILURE)
           fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
       return;
   }

    if (conn->user_data.pwd[strlen(conn->user_data.pwd) - 1] == '/')
        snprintf(new_path, PATH_MAX, "%s%s%s",srv->path, conn->user_data.pwd, arg);
    else
        snprintf(new_path, PATH_MAX, "%s%s/%s",srv->path, conn->user_data.pwd, arg);
    tmp = realpath(new_path, nullptr);
    if (tmp == NULL) {
        if (vector_push_back(conn->sending_buffer, "501 Syntax"
            " error in parameters or arguments.\r\n", 46) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
        return;
    }
    snprintf(new_arg, PATH_MAX, "%s/%s",conn->user_data.pwd, arg);
    memmove(conn->user_data.pwd, new_arg, strlen(new_arg));
    printf("user pwd : %s\n",conn->user_data.pwd);
    if (vector_push_back(conn->sending_buffer, "250\r\n", 5) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
}