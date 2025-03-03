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
    char *new_path = strdup(conn->user_data.pwd);
    if (!arg) {
        if (vector_push_back(conn->sending_buffer,
            "500 Syntax error, command unrecognized.\r\n", 41) == VECTOR_FAILURE)
            fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
        return;
    }
    if (vector_push_back(new_path, arg, sizeof(arg)) == VECTOR_FAILURE) {
        return;
    }
    if (realpath(new_path, conn->user_data.pwd) == NULL) {
        return;
    }
    memset(conn->user_data.pwd, 0, sizeof(conn->user_data.pwd));
    memmove(conn->user_data.pwd, new_path, strlen(new_path));
}