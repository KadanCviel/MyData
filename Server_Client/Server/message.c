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
//  File       : message.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "header.h"


/////////////////////////////////////////////////////////////
// Function: send_message
// Description: Find first delimiter ":", take characters from start until ":" as "name".
//              Find the "name" in list, send msg to that "name".
/////////////////////////////////////////////////////////////
#ifdef USE_SELECT
int send_message(int client_socket[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo)
#else
int send_message(struct pollfd fds[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo)	
#endif /* USE_SELECT */
{
    char format_msg[MAX_MSG_LEN]; // build string to send to client.
	int sd_sendto;                // use in "name:message" destination socket (found from "name") to send to the "message". 
	char f_name[MAX_NAME_LEN];    // temporary name extract from "name:message", use to detect to which socket to send the "msg" 
    int name_found = 0;           // in "name:message" command flag for name found in client socket list.
    char *d_pos = NULL;           // find if ":" exist in the command send from client.
	//d_pos = NULL;
    char *err_column = "Error: The char(:) appears at the beginning (empty name).\r\n\r\n"; 
	char *err_name = "Error:  try to send MSG to non exist client name.\r\n\r\n";
	char *send_client = "Send to Client.\r\n\r\n";
	
    // Search for the first occurrence of ":" in the income message.
    d_pos = strstr(msgs[i],":");
    //name_found = 0;
    if (d_pos != NULL)
    {
        //printf("found--> :\n");
        memset(f_name, 0x00, MAX_NAME_LEN);
                                        
        int index = 0;
        // Copy the first characters from income message to "f_name" until ":"  is reached.
        while (strncmp(&msgs[i][index], ":", 1) != 0)
        {
            f_name[index] = msgs[i][index];
            index++;
        }//End while (strncmp(&msgs[i][index], ":", 1) != 0)
        //printf("Name in message is:%s, length=%d\n",f_name, index);
                                        
        if (index > 0)
        {
            // Find if the name appears in the registered client list. 
            for (int n=0; n < MAX_CLIENTS_NUM; n++)
            {
                //printf("strlen(f_name)=%d, strlen(name[n]=%d\n", strlen(f_name), strlen(name[n]));
                // Compare the names only if the length are squalls,
                // since for not registered names or (disconnected names) the length in the name list is zero,
                // (so we want to skip them).
                if (strlen(f_name) == strlen(name[n]))
                {
                    // Compare the name in the message to the name in the list.
                    if (strncmp(f_name, name[n], strlen(f_name)) == 0) 
                    {
                        // Found, the name exist in the list
                        //printf("There is a match with registered name: %s\n",f_name);
                         name_found = 1;
                                                        
                        // Take from the registered list the socket we need to transfer the message to (index "n").
#ifdef USE_SELECT   
                        sd_sendto = client_socket[n];
#else /* USE_SELECT */
                        sd_sendto = fds[n].fd;                                                        
#endif /* USE_SELECT */
                        // Add the origin name (from which the message came) to message (index "i").
                        prep_msg(name[i], msgs[i], format_msg);
                                                        
                        // Send the message to the desired client.
                        send(sd_sendto, format_msg, strlen(format_msg), 0);
                                                        
                        // Print to server console 
                        printf("Message to Cilent: %s->%s\n", name[i], msgs[i]);
                                                        
                        // Send to the originate client positive indication that send he request performed currectly.
#ifdef USE_SELECT  
						if (client_echo == 1)	
                        {
                            send(client_socket[i], format_msg, strlen(format_msg), 0);
                        }
                        send(client_socket[i], send_client, strlen(send_client), 0);
#else /* USE_SELECT */
						if (client_echo == 1)
                        {
                            send(fds[i].fd, format_msg, strlen(format_msg), 0);
                        }
                        send(fds[i].fd, send_client, strlen(send_client), 0);
#endif /* USE_SELECT */
                    }//End if (strncmp(f_name, name[n], strlen(f_name)) == 0)
                }//End if (strlen(f_name) == strlen(name[n]))
            }//End for(int n=0; n < MAX_CLIENTS_NUM; n++)
                                            
            if (name_found == 0)
            {
#ifdef USE_SELECT   
                printf("Error: Client name(%s) socket(%d) try to send msg to non exist client name.\n",
						name[i], client_socket[i]);
                send(client_socket[i], err_name, strlen(err_name), 0);
#else /* USE_SELECT */
                printf("Error: Client name(%s) socket(%d) try to send msg to non exist client name.\n",
						name[i], fds[i].fd);
                send(fds[i].fd, err_name, strlen(err_name), 0);
#endif /* USE_SELECT */                                                
            }//End if (name_found == 0)
        }//End if (index > 0)
        else
        {
            printf("Error: The char(:) appears at the beginning (empty name).\n");
#ifdef USE_SELECT                                               
            send(client_socket[i], err_column, strlen(err_column), 0);
#else /* USE_SELECT */
            send(fds[i].fd, err_column, strlen(err_column), 0);
#endif /* USE_SELECT */
                                            
        }//End else if (index > 0)
    }//End if (d_pos != NULL)
    else
	{
		return 1;
	}
	
	return 0;
}//End send_message() function