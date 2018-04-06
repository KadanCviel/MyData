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
//  File       : list.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "header.h"


/////////////////////////////////////////////////////////////
// Function: list
// Description: show list of connected clients
/////////////////////////////////////////////////////////////
#ifdef USE_SELECT
void list_of_clients(int socket_id, int client_socket[], int i, char name[][MAX_NAME_LEN])
#else
void list_of_clients(int socket_id, struct pollfd fds[], int i, char name[][MAX_NAME_LEN])	
#endif /* USE_SELECT */ 
{
char st_line[MAX_NAME_LEN];   // temporary string line.
#ifdef USE_SELECT
    printf("\nClient socket(%d) name(%s) request LIST:\n", client_socket[i], name[i]);
#else /* USE_SELECT */
    printf("\nClient socket(%d) name(%s) request LIST:\n", fds[i].fd, name[i]);
#endif /* USE_SELECT */
    for (int j=0; j < MAX_CLIENTS_NUM; j++)
    {
#ifdef USE_SELECT  
        if ((client_socket[j] != 0) && (strlen(name[j]) > 0))
#else /* USE_SELECT */
        if ((fds[j].fd > 0) && (strlen(name[j]) > 0)) 
#endif /* USE_SELECT */                                                                                   
        {
            if (j == i)
            {
                // Add "(me)" after my name, (in this line report).
#ifdef USE_SELECT 
                sprintf(st_line, "%d) name=%s socket=%d (me)\r\n", j+1, name[j], client_socket[j]);
#else /* USE_SELECT */
             sprintf(st_line, "%d) name=%s socket=%d (me)\r\n", j+1, name[j], fds[j].fd);
#endif /* USE_SELECT */
            }//End if (j == i)
            else
            {
                // Add registered name to line report.
#ifdef USE_SELECT 
                sprintf(st_line, "%d) name=%s socket=%d\r\n", j+1, name[j], client_socket[j]);
#else /* USE_SELECT */
                sprintf(st_line, "%d) name=%s socket=%d\r\n", j+1, name[j], fds[j].fd);
#endif /* USE_SELECT */
            }//End else if (j == i)
                                            
            printf("%s", st_line);
                                            
            // Send line report back to cleint how ask for it (by the "LIST:" command).
             send(socket_id, st_line, strlen(st_line), 0); 
        }//End if ((....) && (strlen(name[j]) > 0))
    }//End for(int j=0; j < MAX_CLIENTS_NUM; j++)
}