/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** my_pass.c
*/

#include "myftp.h"
#include "cvector.h"


//230 User logged in, proceed.
//202 Command not implemented, superfluous at this site.
//500 Syntax error, command unrecognized.
//             This may include errors such as command line too long.
//501 Syntax error in parameters or arguments.
//503 Bad sequence of commands.
//421 Service not available, closing control connection.
//             This may be a reply to any command if the service knows it
//             must shut down.
//332 Need account for login.

//PASS
//                  230
//                  202
//                  530
//                  500, 501, 503, 421
//                  332

static void send_bad_sequence_error(peer_t *conn)
{
    if (vector_push_back(conn->sending_buffer,
        "503 Bad sequence of commands.\r\n", 31) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
}

static void handle_anonymous_login(peer_t *conn)
{
    conn->user_data.state = AUTH;
    conn->user_data.pwd[0] = '\0';

    if (vector_push_back(conn->sending_buffer,
        "230 User logged in, proceed.\r\n", 30) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}

static void send_login_incorrect(peer_t *conn)
{
    if (vector_push_back(conn->sending_buffer,
        "530 Login incorrect.\r\n", 22) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
}

void my_pass(server_t *, char *arg, peer_t *conn)
{
    if (conn->user_data.state == AUTH || conn->user_data.state == NOT_AUTH ||
        conn->user_data.state == QUIT) {
        send_bad_sequence_error(conn);
        return;
    }
    if ((!arg && strcmp(conn->user_data.username, "Anonymous") == 0) ||
        (strcmp(conn->user_data.username, "Anonymous") == 0
        && strcmp(arg, "") == 0)) {
        handle_anonymous_login(conn);
    } else {
        send_login_incorrect(conn);
    }
}
