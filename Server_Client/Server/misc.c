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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File       : misc.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "header.h"



/////////////////////////////////////////////////////////////
// Function: help
// Description: format string for sending
/////////////////////////////////////////////////////////////
void help(int socket_id, char* name, int echo, int cr)
{
    char *help_string =
"\r\n\
NAME:new_name\r\n\
Add the <new_name> to server list,\r\n\
so registered clients will be able\r\n\
to send messages to specific client\r\n\
by using its <new_name>.\r\n\
\r\n\
name:message\r\n\
Will send the <message> to specific client <name>.\r\n\
\r\n\
LIST:\r\n\
Will show a list of all registered clients.\r\n\
\r\n\
ALL:message\r\n\
The <message> will broadcast to all registered clients.\r\n\
\r\n\
ECHO:off/on\r\n\
off (0) (default) server will not send back to client received chars as feedback.\r\n\
on  (1)           server send back to client received chars as feedback.\r\n\
\r\n\
CR:off/on\r\n\
off (0)(default) dont show non printable chars, skip <CR><LF>.\r\n\
on  (1)          show all recevied characters include <CR><LF>.\r\n\
\r\n\
BYE:\r\n\
Leave without notice to anyone.\r\n\
\r\n\
EXIT:\r\n\
Inform every one you are going to leave, then leave.\r\n\
\r\n\
ERASE:\r\n\
Erase the registered clients list\r\n\
(everybody need to NAME again!!)\r\n\
\r\n\
";
char st_line[MAX_NAME_LEN];               // temporary string line.
                               
                                    printf("\nReceive HELP request from socket(%d) name(%s).\n", socket_id, name);
                                    send(socket_id, help_string, strlen(help_string), 0);

                                    sprintf(st_line, "\r\nUser_Params:\r\n------------\r\nname=%s\r\nsocket=%d\r\necho=%d\r\ncr=%d\r\n",
        									name, socket_id, echo, cr);
									send(socket_id, st_line, strlen(st_line), 0);

}//End help()



/////////////////////////////////////////////////////////////
// Function:    client_bye
// Description: leave without inform anyone
//              
/////////////////////////////////////////////////////////////
void client_bye(int msgs_last_pos[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int sd)
{
	printf("Client name(%s) socket(%d) say BYE.\n", name[i], sd);

	// Clear the name & msgs.
	memset(msgs[i], 0x00, MAX_MSG_LEN);
	memset(name[i], 0x00, MAX_NAME_LEN);
	msgs_last_pos[i] = 0; 	
}//End client_bye()



/////////////////////////////////////////////////////////////
// Function:    client_exit
// Description: inform everybody then leave
//              
/////////////////////////////////////////////////////////////
#ifdef USE_SELECT
void client_exit(int client_socket[], int msgs_last_pos[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int sd)
#else
void client_exit(struct pollfd fds[], int msgs_last_pos[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int sd)	
#endif /* USE_SELECT */
{
	char format_msg[MAX_MSG_LEN];             // build string to send to client.
	printf("Client name(%s) socket(%d) say EXIT.\n", name[i], sd);
									
	sprintf(format_msg, "\r\n Client name(%s) socket(%d) EXIT now.\r\n",name[i], sd);
                                    
    for(int m=0; m < MAX_CLIENTS_NUM; m++)
    {
        // Do not send to your self or to not in use client socket aray elements.
#ifdef USE_SELECT
        if ((client_socket[m] != 0) && (client_socket[m] != sd))
#else /* USE_SELECT */
        if ((fds[m].fd > 0) && (fds[m].fd != sd))
#endif /* USE_SELECT */                                            
        {
            //printf("Send msg to name[%d]=%s\n", m, name[m]);
#ifdef USE_SELECT                                            
            send(client_socket[m], format_msg, strlen(format_msg), 0);
#else /* USE_SELECT */
            send(fds[m].fd, format_msg, strlen(format_msg), 0);
#endif /* USE_SELECT */                                           
            printf("Message Broadcast(All): %s->%s:%s\n",name[i], name[m], msgs[i]);
        }//End if ((client_socket[m] != 0) && (client_socket[m] != sd))
    }//End for(int m=0; m < MAX_CLIENTS_NUM; m++)			

	// Clear the name & msgs.
	memset(msgs[i], 0x00, MAX_MSG_LEN);
	memset(name[i], 0x00, MAX_NAME_LEN);
	msgs_last_pos[i] = 0; 	
}//End client_exit()



