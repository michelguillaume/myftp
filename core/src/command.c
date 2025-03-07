/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** command.c
*/

#include "myftp.h"
#include "cvector.h"

static const fnct_ptr_t commands[] = {
    {"USER", my_user},
    {"PASS", my_pass},
    {"NOOP", my_noop},
    {"HELP", my_help},
    {"PWD", my_pwd},
    {"CWD", my_cwd},
    {"CDUP", my_cdup},
    {"DELE", my_dele},
    {"PASV", my_pasv},
    {"PORT", my_port},
    {"LIST", my_list},
    {"RETR", my_retr},
    {"STOR", my_stor},
    {"QUIT", my_quit}
};

#ifdef __APPLE__

char *strchrnul(const char *str, int c) {
    const char *ptr = str;

    while (*ptr != '\0' && *ptr != c) {
        ++ptr;
    }
    return (char *)ptr;
}

#endif

void process_command(server_t *srv, peer_t *conn)
{
    char *ptr = strchrnul(conn->receiving_buffer, ' ');

    if (*ptr)
        *ptr = 0;
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strcmp(commands[i].str, conn->receiving_buffer) == 0) {
            commands[i].ptr(srv,
                (VECTOR_BACK(conn->receiving_buffer) == ptr + 1)
                ? nullptr : ptr + 1, conn);
            return;
        }
    }
    if (VECTOR_PUSH_BACK(conn->sending_buffer,
        "500 Syntax error, command unrecognized.\r\n", 41) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push message to sending_buffer\n");
}
