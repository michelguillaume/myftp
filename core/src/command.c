/*
** EPITECH PROJECT, 2025
** myftp
** File description:
** command.c
*/

#include "myftp.h"
#include "cvector.h"

static fnct_ptr_t commands[] = {
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
    /*

    {"QUIT", my_quit},


    {"PASV", my_pasv},
    {"PORT", my_port},
    {"RETR", my_retr},
    {"STOR", my_stor},
    {"LIST", my_list},
     */
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
                (vector_back(conn->receiving_buffer) == ptr + 1)
                ? nullptr : ptr + 1, conn);
            return;
        }
    }
    if (vector_push_back(conn->sending_buffer, "500\r\n", 5) == VECTOR_FAILURE)
        fprintf(stderr, "Error: Failed to push error message to sending_buffer\n");
}
