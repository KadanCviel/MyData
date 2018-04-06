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



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File       : broadcast.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "header.h"


/////////////////////////////////////////////////////////////
// Function: list
// Description: broadcast to all connected clients
/////////////////////////////////////////////////////////////
#ifdef USE_SELECT
void broadcast_all(int sd, int client_socket[], int i, char name[][MAX_NAME_LEN], char *msg, int client_echo)
#else
void broadcast_all(int sd, struct pollfd fds[], int i, char name[][MAX_NAME_LEN], char *msg, int client_echo)	
#endif /* USE_SELECT */ 
{
    char *send_all = "Send to All.\r\n\r\n";
	int b_snd_all = 0;
	char format_msg[MAX_MSG_LEN];             // build string to send to client.
	
	// Prepare the message for sending.
    prep_msg(name[i], msg, format_msg);
                                    
    for(int m=0; m < MAX_CLIENTS_NUM; m++)
    {
        // Do not send to your self or send to not in use client socket 
		// (holes ("null") elements may exist in the array).
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
            printf("Message Broadcast(All): %s->%s:%s\n",name[i], name[m], msg);
             b_snd_all = 1;
        }//End "if ((client_socket[m] != 0) && (client_socket[m] != sd))" SELECT
		// End "if ((fds[m].fd > 0) && (fds[m].fd != sd))" POLL
		
    }//End for(int m=0; m < MAX_CLIENTS_NUM; m++)
                                    
    if (b_snd_all == 1)
    {
        // send indication to originator that broadcast (ALL:) has been performed.
#ifdef USE_SELECT
		if (client_echo == 1)	
        {
            send(client_socket[i], format_msg, strlen(format_msg), 0);
        }//End if (client_echo[i] == 1)
        send(client_socket[i], send_all, strlen(send_all), 0);
#else /* USE_SELECT */
		if (client_echo == 1)	
        {
            send(fds[i].fd, format_msg, strlen(format_msg), 0);
        }//End if (client_echo[i] == 1)                                      
	send(fds[i].fd, send_all, strlen(send_all), 0);
#endif /* USE_SELECT */
    }//End if (b_snd_all == 1)
	
}//End broadcast_all() function