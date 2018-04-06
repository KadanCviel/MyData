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
//  File       : new_name.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "header.h"


/////////////////////////////////////////////////////////////
// Function: set_new_name
// Description: set new name to my client
/////////////////////////////////////////////////////////////
#ifdef USE_SELECT
void set_new_name(int client_socket[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo)
#else
void set_new_name(struct pollfd fds[], int i, char name[][MAX_NAME_LEN], char msgs[][MAX_MSG_LEN], int client_echo)	
#endif /* USE_SELECT */
{
	char *add_name = "Name Added.\r\n\r\n";
	char *fail_name = "Error: Missing name string - only NAME: command detected!\r\n\r\n";
    char *reserve_name = "Error: the name entered is reserve word, so it can't be accepted.\r\n\r\n";
	char f_name[MAX_NAME_LEN]; // temporary name extract from "NAME:new_name", use to detect to which socket to send the "msg" 
    
	// Got registration msg with "NAME:new_name" prepare clean array for "new_name".
    memset(f_name, 0x00, MAX_NAME_LEN);
                                    
    // Extract the new_name string (appears after "NAME:") into name[i] string.
    int ln;
    ln = strlen(msgs[i]);
    //printf("NAME: command length=%d\n", ln);
    if (ln > 5)
    {
        //Extract the "name" from the message.
        for (int r=0; r<(ln-5); r++)
        {
            //if found ":" (hex 0x3A) in new name then take the name until it
            if (msgs[i][r+5]==0x3A)
            {
#ifdef USE_SELECT
                printf("Error: Client socket(%d), the NAME command try to use reserve sign (:) name will be %s.\n",
                        client_socket[i], f_name);
                send(client_socket[i], reserve_name, strlen(reserve_name), 0);
#else /* USE_SELECT */
                printf("Error: Client socket(%d), the NAME command try to use reseve sign (:) name will be %s.\n",
                        fds[i].fd, f_name);
                send(fds[i].fd, reserve_name, strlen(reserve_name), 0);
#endif /* USE_SELECT */
                //stop the four loop
                break;
            }    
                                            
            f_name[r] = msgs[i][r+5];
        }//End for (int r=0; r<(ln-5); r++)
			
        if ( (CompareIgnoreCase(f_name, "HELP")  == 0) ||
             (CompareIgnoreCase(f_name, "LIST")  == 0) ||
             (CompareIgnoreCase(f_name, "NAME")  == 0) ||
			 (CompareIgnoreCase(f_name, "BYE")   == 0) ||
			 (CompareIgnoreCase(f_name, "EXIT")  == 0) ||
			 (CompareIgnoreCase(f_name, "ERASE") == 0) ||
             (CompareIgnoreCase(f_name, "ALL")   == 0) )
        {
#ifdef USE_SELECT   
            printf("Error: Client socket(%d), the NAME command try to use reserve word (%s).\n",
                        client_socket[i], f_name);
            send(client_socket[i], reserve_name, strlen(reserve_name), 0);
#else /* USE_SELECT */
            printf("Error: Client socket(%d), the NAME command try to use reserve word (%s).\n",
                        fds[i].fd, f_name);
            send(fds[i].fd, reserve_name, strlen(reserve_name), 0);                                          
#endif /* USE_SELECT */
        }//End if ( ((CompareIgnoreCase(f_name, "HELP")  == 0) || CompareIgnoreCase(f_name, "LIST")  == 0) ||....
        else
        {
            memset(name[i], 0x00, MAX_NAME_LEN);
            for (int u=0; u < strlen(f_name); u++)
            {
                name[i][u]=f_name[u];
            }//End for (int u=0; u < trlen(f_name); u++)
#ifdef USE_SELECT 
            char tmp_name[300];
			//printf("Socket(%d) add new name=%s\n",client_socket[i],name[i]);
			sprintf(tmp_name, "Socket(%d) add new name=%s\n",client_socket[i],name[i]);
			printf(tmp_name);
			if (client_echo == 1)
            {
                send(client_socket[i], tmp_name, strlen(tmp_name), 0);
            }
            send(client_socket[i], add_name, strlen(add_name), 0);
#else /* USE_SELECT */
            //printf("Socket(%d) add new name=%s\n", fds[i].fd, name[i]);
			char tmp_name[300];
			sprintf(tmp_name, "Socket(%d) add new name=%s\n", fds[i].fd, name[i]);
			printf(tmp_name);
			if (client_echo == 1)
            {
                send(fds[i].fd, tmp_name, strlen(tmp_name), 0);
            }											
            send(fds[i].fd, add_name, strlen(add_name), 0);
#endif /* USE_SELECT */
        }//End else if ( (strncmp(f_name, "HELP", 4) == 0) || ...                                    
    }//End if (ln > 5)
    else
    {
        printf("%s",fail_name);
#ifdef USE_SELECT                                         
        send(client_socket[i], fail_name, strlen(fail_name), 0);
#else /* USE_SELECT */
        send(fds[i].fd, fail_name, strlen(fail_name), 0);
#endif /* USE_SELECT */
    }//End else if (ln > 5)
}//End  set_new_name()