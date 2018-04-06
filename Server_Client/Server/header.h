/*
 * Copyright (c) 2018, Kadan. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Kadan or the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */ 



#include <stdio.h>
#include <string.h>       //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>       //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <poll.h> //struct pollfd
#include <ctype.h> //tolower()


#define USE_SELECT 
//Allow the define above definition will cause to work with socket "select()" function (the old way)
//Undefined above definition will cause to work with socket "poll()" function (instead of "select()")



#define TRUE   1
#define FALSE  0
#define DEFAULT_MASTER_PORT 8888
#define MAX_CLIENTS_NUM 500 //was 100
#define MAX_MSG_LEN 1024
#define MAX_NAME_LEN 100
#define MIN_PORT_NUM 2000
#define MAX_PORT_NUM 65535


// File: utils.c
void prep_msg(char* from, char* in_msg, char* out_msg);
int CompareIgnoreCase(char* s1, char* s2);


// File:list.c
#ifdef USE_SELECT
void list_of_clients(int socket_id, int client_socket[], int i, char name[][MAX_NAME_LEN]);
#else
void list_of_clients(int socket_id, struct pollfd fds[], int i, char name[][MAX_NAME_LEN]);
#endif /* USE_SELECT */

// File: broadcast.c
#ifdef USE_SELECT
void broadcast_all(int sd, int client_socket[], int i, char name[][MAX_NAME_LEN], char *msg, int client_echo);
#else
void broadcast_all(int sd, struct pollfd fds[], int i, char name[][MAX_NAME_LEN], char *msg, int client_echo);
#endif /* USE_SELECT */

// File:new_name.c
#ifdef USE_SELECT
void set_new_name(int client_socket[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo);
#else
void set_new_name(struct pollfd fds[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo);
#endif /* USE_SELECT */


// File: message.c
#ifdef USE_SELECT
int send_message(int client_socket[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo);
#else
int send_message(struct pollfd fds[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo);	
#endif /* USE_SELECT */


// File: misc.c
void help(int socket_id, char* name, int echo, int cr);
void client_bye(int msgs_last_pos[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int sd);

#ifdef USE_SELECT
void client_exit(int client_socket[], int msgs_last_pos[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int sd);
#else
void client_exit(struct pollfd fds[], int msgs_last_pos[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int sd);
#endif /* USE_SELECT */








