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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File       : main.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
//
//
// Change the "echo" behavior to "chat" behavior, (reserve commands are "NAME:", "LIST:" "BYE", "EXIT", "ALL", ""ERASE" & "HELP:")
// Run the application server from "cmd" window with server.exe "port_num"
// Run the client  by: telent "ip_addr" (or "localhost") "port_num"
//                     or client.exe "ip_addr" "port_num"
// (same "port_num" in server and client)
//
//
// NAME:new_name     --> will register (add) the "new_name" to server list, so registered clients will be able
//                       to send messages to specific client (using "new_name").
//
// name:message      --> Will send a "message" to specific client("name").
//
// LIST:             --> Will show a list of all registered clients.
//
// ALL:message       -->"message" will broadcast to all registered clients.
//                    
// HELP:             --> Give above explanation.
//
// ECHO:              on/off (on = server send back received chars as feedback) 
//                           (off= server will not send back received chars as feedback).
// BYE:               leave without notice to anyone.
// EXIT:              inform every one you are going to leave, then leave.
// ERASE:             ERASE the registered clients list (everybody need to NAME again!!)
//
//
// compile use sygwin gcc compiler
//$cd C:/mydata/Tsviel/Projects/Chat/05_Server_and_Client_2017_04_10/Server/
//$gcc -o server server.c
//
//
//
//$ cd C:\mydata\Tsviel\Projects\Chat\06_Server_and_Client\Server
//$ make
//$ make clean
//
//$./server.exe PORT_NUM    (if the parameter is missing it will use 8888 as PORT_NUM)
//$./server.exe 7777
// use cygwin1.dll (take sygwin dll x86  and not 64_x86 dll)in the same folder. 
// from cmd> line.
// >server.exe PORT_NUM
//
//
//
// Code base on: 
// Example code: A simple server side code, which echo back the received message.
// Handle multiple socket connections with select and fd_set on Linux
// link:
// http://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
// https://linux.die.net/man/2/select_tut
// https://www.tutorialspoint.com/unix_sockets/unix_sockets_pdf_version.htm
// http://developerweb.net/viewtopic.php?pid=12590
// http://www.ibm.com/support/knowledgecenter/ssw_i5_54/rzab6/xnonblock.htm
// poll
// http://www.ulduzsoft.com/2014/01/select-poll-epoll-practical-difference-for-system-architects/
// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "header.h"



/////////////////////////////////////////////////////////////
// Function: main
// Description: main program loop (receive and send messages)
/////////////////////////////////////////////////////////////    
int main(int argc , char *argv[])  
{  
    char *greeting_message = "\r\nWelcome to Chat Server v1.4\r\n\r\n";  //new connection greeting message
    printf("%s", greeting_message);
    int opt = TRUE;                           // use for output from setsockopt().
    int master_socket;                        // detect  new connection use in: socket(),setsockopt(),bind(),listen().
    int addrlen;                              // use in accept().
    int new_socket;                           // return from accept() for each new client connection then store in client_socket[] array.
    int activity;                             // return from select() to detect activity on one of the sockets.
    int i;                                    // for loop scan all the clients. 
    int valread;                              // return from read() how many bytes were receive.
    int sd;                                   // use to hold socket_descriptor in many cases e.g. when building SET list, read() data from socket, send socket in LIST command.
    int master_port_num;                      // use in the bind() & listen() to listen to master port.
    int send_ret;                             // use the send() greeting back to income connection from new client.
    struct sockaddr_in address;               // use in bind() & accept() commands.
    char buffer[MAX_MSG_LEN];                 // data buffer of 1K	
    int client_echo[MAX_CLIENTS_NUM];         // add client_echo[] array to save the echo on/off value per client. (not as global parameter)
                                              // 0=no echo. just print "Send to Client".  1=echo the send command back to sender & also  print "Send to Client"
    int client_cr[MAX_CLIENTS_NUM];           // 0= skip none printable chars (remove <CR><LF> from message), 1=show all chars "as is"
											  
    char *full_message = "client list is full, can't support you (try again later).\r\n";
    char *fail_string = "Fail to parse the message.\r\n\r\n";
    char *err_buff_acum = "Error: Can not handle MSGS with accumulate bigger length than the buffer can hold.\r\n\r\n";
    char *err_rx_line = "Error: Can not handle income read line from socket equals to MAX_MSG_LEN.\r\n\r\n";
    char msgs[MAX_CLIENTS_NUM][MAX_MSG_LEN];  // Store the message from client(A) to client(B), filed in runtime (char by char/or by line). 
    int msgs_last_pos[MAX_CLIENTS_NUM];       // Where the last character entered, in msg two-dimentional array.
    char name[MAX_CLIENTS_NUM][MAX_NAME_LEN]; // the register name same in same array offset as the socket where it came from .
    char format_msg[MAX_MSG_LEN];             // build string to send to client.
    int sock_opt_ret;                         // use in setsockopt().
    int bind_ret;                             // use in bind().
    int b_end_found;
    memset(buffer, 0x00, MAX_MSG_LEN);        // use in read(), especially important buffer[0] as the income char on each press in Telent.  
    
#ifdef USE_SELECT
    int max_sd;                               // use in select().
    int client_socket[MAX_CLIENTS_NUM];       // socket[] use to send  data to client, or use to send back indication to socket where the command came from.  
    fd_set readfds;                           // use in SET of socket descriptors 
    printf("The Server is using select() method\n");
#else 
    //The structure for MAX_CLIENTS_NUM events for use in poll()
    struct pollfd fds[MAX_CLIENTS_NUM];
    int    max_index;
    printf("The Server is using poll() method\n");
#endif /* USE_SELECT */
    
    
    // Initial all client_socket[]'s, Messages &name's to 0x00, (to mark them as "not filled").
    for (i = 0; i < MAX_CLIENTS_NUM; i++)
    {
        memset(msgs[i], 0x00, MAX_MSG_LEN);
        memset(name[i], 0x00, MAX_NAME_LEN);
        msgs_last_pos[i] = 0;
        client_echo[i] = 0; //0=no echo. just print "Send to Client".  1=echo the send command back to sender & also  print "Send to Client"
		client_cr[i]=0; //0=skip non printable chars remove <CR><LF> from message
#ifdef USE_SELECT
        client_socket[i] = 0;
#else  
        // clean poll() structure
        // http://man7.org/linux/man-pages/man2/poll.2.html
        // If this field is negative, then the corresponding events field is ignored and the r_events field returns zero.
        fds[i].fd = -1;
#endif /* USE_SELECT */
    }//End for (i = 0; i < MAX_CLIENTS_NUM; i++)
    
    ///////////////////////////////
    // Create the master socket. //
    ///////////////////////////////
    master_socket = socket(AF_INET, SOCK_STREAM , 0);
    if (master_socket == 0)
    {  
        printf("Error: Call to socket() API failed, can not create the master_socket.\n");
        exit(EXIT_FAILURE);
    }//End if (master_socket == 0)
    
    ////////////////////////////////////////////////////////////////////////
    // Set master socket with "setsockopt" to allow multiple connections. //
    ////////////////////////////////////////////////////////////////////////
    sock_opt_ret = setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    if(sock_opt_ret < 0)
    {
        printf("Error: Call setsockopt() failed, (for master_socket return:%d).\n", sock_opt_ret);
        exit(EXIT_FAILURE);
    }//End if(sock_opt_ret < 0)
    
    //////////////////////////////////////////////////////
    // Set type of socket created to TCP/IP socket type //
    //////////////////////////////////////////////////////
    address.sin_family = AF_INET;         //for Internet TCP/IP socket type 
    address.sin_addr.s_addr = INADDR_ANY; //set IP address to default "localhost" (127.0.0.1)
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Get the master socket "port_num" value from user input, (from command line first argument). //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if (argc > 1)
    {
        master_port_num = 0;
        master_port_num = atoi(argv[1]);
        if ((master_port_num < MIN_PORT_NUM) || (master_port_num > MAX_PORT_NUM))
        {
            printf("Port number %d is not in the expected range[%d,%d], using default port:%d.\n",
                   master_port_num, MIN_PORT_NUM, MAX_PORT_NUM, DEFAULT_MASTER_PORT);
            master_port_num = DEFAULT_MASTER_PORT;  //e.g. 8888 
        }//End if ((master_port_num < MIN_PORT_NUM) || (master_port_num > MAX_PORT_NUM))
    }//End if (argc > 1)
    else
    {
       master_port_num = DEFAULT_MASTER_PORT;  //e.g. 8888
    }//End else if (argc > 1)
    
    address.sin_port = htons(master_port_num);  //e.g. default 8888
    
    ///////////////////////////////////////////////////////////////////
    // Bind the master socket to "localhost" ip and master port num. //
    ///////////////////////////////////////////////////////////////////
    bind_ret = bind(master_socket, (struct sockaddr *)&address, sizeof(address));
    if (bind_ret < 0)
    {
        printf("Error: Call bind() API failed (master_socket returned:%d).\n", bind_ret);
        exit(EXIT_FAILURE);
    }//End if (bind_ret < 0)
    
    printf("Listener on port %d\n", master_port_num);
    
    ////////////////////////////////////////////////////////////////////////////
    // Try to specify maximum of 3 pending connections for the master socket. //
    ////////////////////////////////////////////////////////////////////////////
    if (listen(master_socket, 3) < 0)
    {
        printf("Error: listen() to master_socket failed.\n");
        exit(EXIT_FAILURE);
    }//End if (listen(master_socket, 3) < 0)
    
    
    
#ifdef USE_SELECT
#else    
    // code below need when working with poll()
    // Monitor master_socket for input
    fds[0].fd = master_socket;
    fds[0].events = POLLIN;
    max_index = 1;
#endif /* USE_SELECT */
    
    addrlen = sizeof(address);
    
    printf("Waiting for connections ...\n");
    /////////////////////////////////////////////////////
    // THE MAIN LOOP:  accept the incoming connection. //
    /////////////////////////////////////////////////////
    
    while(TRUE)
    {
        /*************************************************
        * USE SELECT API - BEGIN
        **************************************************/
#ifdef USE_SELECT
        //NOTE: after using the select() need to empty the fd_list structure and build it again, for next select()
        //(since the select alter the fd_list structure in a way it can not reuse).
        
        ///////////////////////////////////////
        // Clear the socket SET (read list). //
        ///////////////////////////////////////
        FD_ZERO(&readfds);
        
        ///////////////////////////////////////////
        // Add master socket to SET (read list). //
        ///////////////////////////////////////////
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;  
        
        // Add childes sockets to SET. 
        // This code is important, after the clients send their first messages.
        // so after each receive message (and complete it's analysis), 
        // we start again waiting for the next message form the clients,
        // by building the SET again, including again the connected clients.
        for (i = 0; i < MAX_CLIENTS_NUM; i++)
        {
            // Socket descriptor.
            sd = client_socket[i];
            
            // If valid socket descriptor, then add the socket to the SET (read list).
            if (sd > 0)
            {
                // Add already connected socket to SET (read list).
                FD_SET(sd, &readfds);
                
                // Find the highest file descriptor number, (we will need it for the select() function).
                if (sd > max_sd) 
                {
                    max_sd = sd;
                }//End  if (sd > max_sd)
            }//End if (sd > 0)
        }//End for (i = 0; i < MAX_CLIENTS_NUM; i++)
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Select use to wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely. //
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        activity = select(max_sd+1, &readfds, NULL, NULL, NULL);
        
        if ((activity < 0) && (errno != EINTR))
        {
            printf("Error: select() return error activity=%d, errno=%d.\n", activity, errno);
        }//End if ((activity < 0) && (errno != EINTR))
        else
        {
            // If something happened on the master socket, then it's an incoming new connection,
            // (if it is not on the master_socket then it's new data for already existing connection).
            if (FD_ISSET(master_socket, &readfds))
            {
                /////////////////////////////////////////////////////
                // check the master socket, handle new connection. //
                /////////////////////////////////////////////////////
                new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                if (new_socket < 0)
                {
                    printf("Error: accept() new socket via master socket failed.\n");
                    exit(EXIT_FAILURE);
                }//End if (new_socket < 0)
                
                // Inform user of socket number - used in send and receive commands.
                printf("New connection, socket:%d, ip:%s, port:%d\n",
                        new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                
                ////////////////////////////////////////////////////////////////
                // Send "Welcome" (greeting message) to connected new socket. //
                ////////////////////////////////////////////////////////////////
                send_ret = send(new_socket, greeting_message, strlen(greeting_message), 0);
                
                if (send_ret != strlen(greeting_message))
                {
                    printf("Error: Greeting send return value (%d) which not equal to greeting length (%ld).\n", send_ret, strlen(greeting_message));
                }//End if (send_ret != strlen(greeting_message))
                else
                {
                    printf("Welcome message sent successfully\n");
                }//End else if(send_ret != strlen(greeting_message))
                
                /////////////////////////////////////////////
                // Add the new socket to array of sockets. //
                /////////////////////////////////////////////
                for (i = 0; i < MAX_CLIENTS_NUM; i++)
                {   
                    // If position is vacant.
                    if (client_socket[i] == 0)
                    {
                        // For reading new data from a new socket. We need to add the new socket into the active clients socket list.
                        client_socket[i] = new_socket; 
                        printf("Adding to list of sockets at position:%d socket:%d.\n", i, new_socket);
                        break;
                    }//End if (client_socket[i] == 0)
                     
                    // If income data is from socket already exist in the list(this is unexpected to happened - just monitoring)
                    if (client_socket[i] == new_socket)
                    {
                        // Not likely to happened, but in case it happened...
                        // Exit the loop, no point in continue the search,
                        // since income data is from already exist in the list socket. not expected to happened since new data
                        // should come on already exist in the list socket and not from the master socket.
                        printf("NOTICE: Income data is from already exist in the list socket index=%d, socket=%d\n", i, new_socket); 
                        break;
                    }
                }//End for (i = 0; i < MAX_CLIENTS_NUM; i++)
                
                if (i == MAX_CLIENTS_NUM)
                {
                    printf("Error: Max client socket index reached, but new income socket %d could not add since list is full.\n", new_socket);
                    
                    // Inform the client it can't connect since active clients socket list is full.
                    send(new_socket, full_message, strlen(full_message), 0);
                    //exit(EXIT_FAILURE); 
                }//End if (i == MAX_CLIENTS_NUM)
               
            }//End if (FD_ISSET(master_socket, &readfds))
            
            // we reach here in cases:
            // The "select()" wait was exited, 
            // 1. FD_ISSET(master_socket..) return "false" (means the income data is not on master socket
            //    its from already connected socket in the list) 
            // 2. FD_ISSET(master_socket..) return "true" (means the income data is on master socket)
            //    and the loop exited, after adding new_socket to list, need to handle the income data.
            
            // so for both cases above need to handle the income data message from client,
            // by looping in all already connected sockets and find from where the message is coming
            for (i = 0; i < MAX_CLIENTS_NUM; i++)
            {
                
                sd = client_socket[i];
                
                // if sd is 0 then it mean that this array element is not in use, so there is no point to do more work on it.
                if (sd == 0) 
                {
                    // go to next index i in the loop.
                    //printf("continue index(%d)\n", i);
                    continue;
                }
                // Check from which socket in the list the data arrived, by checking all the connected sockets in the list 
                if (FD_ISSET(sd, &readfds))
                {
                    
                    // We found from where the data come, so need to read it.
                    memset(buffer, 0x00, MAX_MSG_LEN);
                    
                    // Check if it was for closing, (when the return read length is zero), 
                    // else read the incoming message (MAX_MSG_LEN is 1024).
                    // TODO: if the message length is equals the MAX_MSG_LEN, there should be more read()'s?
                    valread = read(sd, buffer, MAX_MSG_LEN);
                    
                    if (valread < 0)
                    {
                            printf("Error: read() return negative value(%d) for socket(%d).\n", valread, sd);
                            
                            // Close the socket and mark it place with 0 in sockets list 
                            // and clean this socket msgs & name arrays for later reuse.
                            close(sd);
                            client_socket[i] = 0;
                            
                            // Clear the name & msgs.
                            memset(msgs[i], 0x00, MAX_MSG_LEN);
                            memset(name[i], 0x00, MAX_NAME_LEN);
                            msgs_last_pos[i] = 0;                 
                    }
                    else if (valread == 0)
                    {
                        // Somebody disconnected, get his details and print.
                        printf("Client name(%s) socket(%d) disconnected.\n", name[i], sd);
                        
                        // Close the socket and mark it place with 0 in sockets list
                        // and clean this socket msgs & name arrays for later reuse.
                        close(sd);
                        
                        client_socket[i] = 0;
                        
                        // Clear the name & msgs.
                        memset(msgs[i], 0x00, MAX_MSG_LEN);
                        memset(name[i], 0x00, MAX_NAME_LEN);
                        msgs_last_pos[i] = 0; 
                    }//End else if (valread == 0)
                    else if (valread == MAX_MSG_LEN)
                    {
                        // TODO: if the message length is equal the MAX_MSG_LEN, there should be more read()'s?
                        
                        printf("Error: Can not handle income read line from socket equal to MAX_MSG_LEN (%d).\n", MAX_MSG_LEN);
                        
                        send(client_socket[i], err_rx_line, strlen(err_rx_line), 0);
                    }
                    ////////////////////////////////////    
                    //Handle the message that came in //
                    ////////////////////////////////////
                    else
                    {
                        // Set the string terminating NULL byte on the end of the data read.
                        // TODO: If we will implement "file reading"  we need todo things diffently here? ...
                        
                        // Add the new character/s, but do no overflow the msgs[] max size.
                        if ((strlen(msgs[i])+valread) < MAX_MSG_LEN)
                        {
                            // Take only the first character from arrived message, 
                            // since Microsoft Telnet send the typed chars one by one
                            // but android telnet send line of text (few chars together)
                            // and Telnet sometimes added some extra data (e.g. zeros or none printable characters)
                            // we don't want to add to the message the none printable characters.
                            // First: accumulate all the typed characters into the buffer.
                            // Second: scan and insert only printable characters into the msgs.
                            
                            b_end_found = 0;
                            
                            for (int x=0; x < valread; x++)
                            {
                                if ( (client_cr[i] == 0) && (buffer[x] == 13) )
                                {
                                    b_end_found = 1;
                                    break;
                                }//End if ( (client_cr[i] == 0) && (buffer[x] == 13) )
                                
                                
                                if (client_cr[i] == 1)
                                {
                                    msgs[i][msgs_last_pos[i]] = buffer[x];
                                    msgs_last_pos[i]++;
                                }//End if (client_cr[i] == 1)								
								else if ( (buffer[x]>=32) && (buffer[x]<=127) )
                                {
                                    // if (client_cr[i] == 0) take only printable characters 
									msgs[i][msgs_last_pos[i]] = buffer[x];
                                    
                                    msgs_last_pos[i]++;
                                }//End if ( (buffer[x]>=32) && (buffer[x]<=127) )
                            }//End for (int x=0; x<valread; x+=)
                            
                            
                            // Search if CR arrived (Carriage Return value is 0x0d) sign us for end-of-message
                            if ((buffer[0] == 0x0d) ||(b_end_found == 1))
                            {                                
                                // Since this is end-of-message we will look into it to see which command arrived.
                                // Check if this message is:
                                // 1. HELP:
                                // 2. LIST:            (Show registered client name in a list)
                                // 3. ECHO:on/off 
								//    CR:on/off
                                // 4. BYE:
                                // 5. EXIT:
                                // 6. ALL:message      (Broadcast to all registered clients)
                                // 7. NAME:new_name    (Registration)
                                // 8. name:message     (Send message to other registered client)
                                // 9. Parse Error 
                                
                                
                                //////////////////
                                // Handle HELP: //
                                //////////////////
                                if (CompareIgnoreCase(msgs[i], "HELP:") == 0)
                                {
                                    help(client_socket[i], name[i], client_echo[i], client_cr[i]);
                                }// End if (CompareIgnoreCase(msgs[i], "HELP:") == 0)
                                
                                
                                //////////////////
                                // Handle LIST: //
                                //////////////////
                                else if (CompareIgnoreCase(msgs[i], "LIST:") == 0)
                                {
                                    list_of_clients(sd, client_socket, i, name);	
                                }//Endif (CompareIgnoreCase(msgs[i], "LIST:") == 0)
                                
                                
                                ////////////////////////
                                // Handle ECHO:on/off //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "ECHO:") == 0)
                                {
                                    int ln;
                                    ln = strlen(msgs[i]);
                                    char on_off[3];
                                    //printf("ECHO: command length=%d\n", ln);
                                    if (ln >= 7)
                                    {
                                        //Extract the "on"/"off" parameter from the message.
                                        for (int r=0; r<(ln-5); r++)
                                        {
                                            on_off[r] = msgs[i][r+5];
                                        }//End for (int r=0; r<(ln-5); r++)
                                        
                                        if (CompareIgnoreCase(on_off, "on") == 0)
                                        {
                                            client_echo[i] = 1;
											printf("client %d echo is on(1)\r\n",i);
                                        }
                                        
                                        if (CompareIgnoreCase(on_off, "off") == 0)
                                        {
                                            client_echo[i] = 0;
											printf("client %d echo is off(0)\r\n",i);
                                        }
                                    }
                                }//End else if if (CompareIgnoreCase(msgs[i], "ECHO:") == 0)
                                
                                
								
                                //////////////////////
                                // Handle CR:on/off //
                                //////////////////////
                                else if (CompareIgnoreCase(msgs[i], "CR:") == 0)
                                {
                                    int ln;
                                    ln = strlen(msgs[i]);
                                    char cr_off[3];
                                    //printf("CR: command length=%d\n", ln);
                                    if (ln >= 5)
                                    {
                                        //Extract the "on"/"off" parameter from the message.
                                        for (int r=0; r<(ln-3); r++)
                                        {
                                            cr_off[r] = msgs[i][r+3];
                                        }//End for (int r=0; r<(ln-5); r++)
                                        
                                        if (CompareIgnoreCase(cr_off, "on") == 0)
                                        {
                                            client_cr[i] = 1;
											printf("client %d cr is on(1)\r\n",i);
                                        }
                                        
                                        if (CompareIgnoreCase(cr_off, "off") == 0)
                                        {
                                            client_cr[i] = 0;
											printf("client %d cr is off(0)\r\n",i);
                                        }
                                    }
                                }//End else if if (CompareIgnoreCase(msgs[i], "ECHO:") == 0)								
								

                                ////////////////////////
                                // Handle BYE:        //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "BYE:") == 0)
                                {
                                    client_bye(msgs_last_pos, i, name, msgs, sd);
                                }//End else if (CompareIgnoreCase(msgs[i], "BYE:") == 0)
                                
                                
                                ////////////////////////
                                // Handle EXIT:       //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "EXIT:") == 0)
                                {
                                    client_exit(client_socket, msgs_last_pos, i, name, msgs, sd);
                                }//End else if (CompareIgnoreCase(msgs[i], "EXIT:") == 0)
                                
                                
                                ////////////////////////
                                // Handle ERASE:       //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "ERASE:") == 0)
                                {
                                    printf("Client name(%s) socket(%d) ERASE the list (everybody need to NAME again!!).\n", name[i], sd);
                                    
                                    sprintf(format_msg, "\r\n Client name(%s) socket(%d) ERASE the list (everybody need to NAME again!!).\r\n",name[i], sd);
                                    
                                    for(int m=0; m < MAX_CLIENTS_NUM; m++)
                                    {                                          
                                        send(client_socket[m], format_msg, strlen(format_msg), 0);
                                        
                                        // Clear the name & msgs.
                                        memset(msgs[m], 0x00, MAX_MSG_LEN);
                                        memset(name[m], 0x00, MAX_NAME_LEN);
                                        msgs_last_pos[m] = 0; 
                                    }//End for(int m=0; m < MAX_CLIENTS_NUM; m++)
                                }//End else if (CompareIgnoreCase(msgs[i], "ERASE:") == 0)
                                
                                
                                ////////////////////////
                                // Handle ALL:message //
                                ////////////////////////
                                // If four first letter are "ALL:" broadcast the message to All the clients.
                                else if (CompareIgnoreCase(msgs[i], "ALL:") == 0)
                                {
                                    broadcast_all(sd, client_socket, i, name, msgs[i], client_echo[i]);
                                }//End if (CompareIgnoreCase(msgs[i], "ALL:") == 0)
                                
                                
                                ////////////////////////////
                                // Handle NAME:new_name   //
                                ////////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "NAME:") == 0)	
                                {
                                    set_new_name(client_socket, i, name, msgs, client_echo[i]);
                                }//End if (CompareIgnoreCase(msgs[i], "NAME:") == 0)
                                else
                                {
                                    ///////////////////////////
                                    // Handle name:message   //
                                    ///////////////////////////
                                    int send_ret = 0;
                                    send_ret = send_message(client_socket, i, name, msgs, client_echo[i]);
                                    ///////////////////////////////////////
                                    // Handle send message Parse Error   //
                                    ///////////////////////////////////////
                                    // Name not found missing separator or no name was specify (no "NAME:" string).
                                    if (send_ret != 0)
                                    {
                                        // Wrong command  (did not find the name or no name specified).
                                        // Note: Sometimes using back key during editing are not handle properly, so e.g. then find name will fail.
                                        msgs[i][msgs_last_pos[i]-1] = 0x00;
                                        printf("Receive message: %s, from socket(%d) name(%s), could not parse correctly.\n",
                                               msgs[i], client_socket[i], name[i]);
                                        send(client_socket[i], fail_string, strlen(fail_string), 0);
                                    }//End  if (send_ret != 0)
                                    
                                }//End else if (CompareIgnoreCase(msgs[i], "NAME:") == 0)
                                
                                // After performing the command, clean the accumulate storage place, 
                                // to enable fresh start for the next command from this client.
                                memset(msgs[i], 0x00, MAX_MSG_LEN);
                                msgs_last_pos[i] = 0;
                            }//End if ((buffer[0] == 0x0d) ||(b_end_found == 1))
                            
                        }//End if ((strlen(msgs[i])+valread) < MAX_MSG_LEN)
                        else
                        {
                            // TODO:
                            // Handle msgs with bigger length than the buffer can hold ...
                            printf("Error: Can not handle msgs with accumulate bigger length than the buffer can hold.\n");
                            send(client_socket[i], err_buff_acum, strlen(err_buff_acum), 0);
                        }//End else if ((strlen(msgs[i])+1) < MAX_MSG_LEN)
                    }//End else of "else if (valread == MAX_MSG_LEN)"
                }//End if (FD_ISSET(sd, &readfds))
            }//End for (i = 0; i < MAX_CLIENTS_NUM; i++)
        }//End 	else if ((activity < 0) && (errno != EINTR))
        /*************************************************
        * USE SELECT API - END
        **************************************************/	
        
#else /* USE_SELECT */
        
        /*************************************************
        * USE POLL API - BEGIN
        **************************************************/
        ///////////////////////////////////////////////////////////////////////////////////////////////
        //use poll() to wait for an activity on one of the sockets timeout wait 10[sec] for activity //
        ///////////////////////////////////////////////////////////////////////////////////////////////
        activity = poll(fds, max_index, 10000); //wait 10[sec] for activity
        
        if (activity == -1)
        {
            // report error and abort 
            printf("Error: poll() activity return -1\n");
            //exit(EXIT_FAILURE);
        }
        else if (activity == 0)
        {
            // timeout: no event detected
            printf("Error: poll() activity return 0 (timeout)\n");
            //exit(EXIT_FAILURE);
        }
        else
        {
            // If we detect input event on fds[0], which is the master_socket. 
            // set it to zero, so we can reuse the structure in future.
            if (fds[0].revents & POLLIN)
            {
                fds[0].revents = 0;
                
                /////////////////////////////////////////////////////
                // check the master socket, handle new connection. //
                /////////////////////////////////////////////////////
                new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                if (new_socket < 0)
                {
                    printf("Error: accept() new socket via master socket failed.\n");
                    exit(EXIT_FAILURE);
                }//End if (new_socket < 0)
                
                // Inform user of socket number - used in send and receive commands.
                printf("New connection, socket:%d, ip:%s, port:%d\n",
                        new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                
                ////////////////////////////////////////////////////////////////
                // Send "Welcome" (greeting message) to connected new socket. //
                ////////////////////////////////////////////////////////////////
                send_ret = send(new_socket, greeting_message, strlen(greeting_message), 0);
                
                if (send_ret != strlen(greeting_message))
                {
                    printf("Error: Greeting send return value (%d) which not equal to greeting length (%ld).\n", send_ret, strlen(greeting_message));
                }//End if (send_ret != strlen(greeting_message))
                else
                {
                    printf("Welcome message sent successfully\n");
                }//End else if(send_ret != strlen(greeting_message))
                
                /////////////////////////////////////////////////////////
                // Use poll(), add the new socket to array of sockets. //
                /////////////////////////////////////////////////////////
                for (i = 1; i < MAX_CLIENTS_NUM; i++)
                {
                    // If position is vacant.
                    if (fds[i].fd == -1)
                    {
                        fds[i].fd = new_socket;
                        fds[i].events = POLLIN;
                        if ( i > (max_index-1) )
                        {
                            max_index = i+1;
                        }
                        //exit the for loop
                        break;
                    }//End if (fds[i].fd == -1)
                    
                    if  (fds[i].fd == new_socket)
                    {
                        printf("NOTICE: Income data is from already exist in the list socket index=%d, socket=%d\n", i, new_socket); 
                        //exit the for loop
                        break;
                    }//End if  (fds[i].fd == new_socket)
                }//End  for (i = 1; i < MAX_CLIENTS_NUM; i++)
                
                if (i == MAX_CLIENTS_NUM)
                {
                    printf("Error: Max client socket index reached, but new income socket %d could not add since list is full.\n", new_socket);
                    
                    // Inform the client it can't connect since active clients socket list is full.
                    send(new_socket, full_message, strlen(full_message), 0);
                    //exit(EXIT_FAILURE); 
                }//End if (i == MAX_CLIENTS_NUM)
               
            }//End if (pfd[0].revents & POLLIN)"
            
            // we reach here in cases:
            // The "select()" wait was exited, 
            // 1. FD_ISSET(master_socket..) return "false" (means the income data is not on master socket
            //    its from already connected socket in the list) 
            // 2. FD_ISSET(master_socket..) return "true" (means the income data is on master socket)
            //    and the loop exited, after adding new_socket to list, need to handle the income data.
            
            // so for both cases above need to handle the income data message from client,
            // by looping in all already connected sockets and find from where the message is coming
            for (i = 0; i < MAX_CLIENTS_NUM; i++)
            {
             
                sd = fds[i].fd;
                // if sd is -1 then it mean that this array element is not in use, so there is no point to do more work on it.
                if (sd == -1) 
                {
                    // go to next index i in the loop.
                    //printf("continue index(%d)\n", i);
                    continue;
                }//End if (sd == -1)
                
                // Check from which socket in the list the data arrived, by checking all the connected sockets in the list 
                
                // If we detect the event, zero it out so we can reuse the structure
                if (fds[i].revents & POLLIN)
                {
                   fds[i].revents = 0;
                   
                    // We found from where the data come, so need to read it.
                    memset(buffer, 0x00, MAX_MSG_LEN);
                    
                    // Check if it was for closing, (when the return read length is zero), 
                    // else read the incoming message (MAX_MSG_LEN is 1024).
                    // TODO: if the message length is equals the MAX_MSG_LEN, there should be more read()'s?
                    valread = read(sd, buffer, MAX_MSG_LEN);
                    
                    if (valread < 0)
                    {
                            printf("Error: read() return negative value(%d) for socket(%d).\n", valread, sd);
                            
                            // Close the socket and mark it place with 0 in sockets list 
                            // and clean this socket msgs & name arrays for later reuse.
                            close(sd);
                            
                            // mark that this fd (in index i ) is not in use 
                            fds[i].fd = -1;          
                            
                            // Clear the name & msgs.
                            memset(msgs[i], 0x00, MAX_MSG_LEN);
                            memset(name[i], 0x00, MAX_NAME_LEN);
                            msgs_last_pos[i] = 0;                 
                    
                    }
                    else if (valread == 0)
                    {
                        // Somebody disconnected, get his details and print.
                        printf("Client name(%s) socket(%d) disconnected.\n", name[i], sd);
                        
                        // Close the socket and mark it place with 0 in sockets list
                        // and clean this socket msgs & name arrays for later reuse.
                        close(sd);
                        
                        // mark that this fd (in index i ) is not in use anymore
                        fds[i].fd = -1;
                        
                        // Clear the name & msgs.
                        memset(msgs[i], 0x00, MAX_MSG_LEN);
                        memset(name[i], 0x00, MAX_NAME_LEN);
                        msgs_last_pos[i] = 0; 
                    }//End else if (valread == 0)
                    else if (valread == MAX_MSG_LEN)
                    {
                        // TODO: if the message length is equal the MAX_MSG_LEN, there should be more read()'s?
                        
                        printf("Error: Can not handle income read line from socket equal to MAX_MSG_LEN (%d).\n", MAX_MSG_LEN);
                        
                        send(fds[i].fd, err_rx_line, strlen(err_rx_line), 0);
                    }
                    ////////////////////////////////////    
                    //Handle the message that came in //
                    ////////////////////////////////////
                    else
                    {
                        // Set the string terminating NULL byte on the end of the data read.
                        // TODO: If we will implement "file reading"  we need todo things diffently here? ...
                        
                        // Add the new character/s, but do no overflow the msgs[] max size.
                        if ((strlen(msgs[i])+valread) < MAX_MSG_LEN)
                        {
                            // Take only the first character from arrived message, 
                            // since Microsoft Telnet send the typed chars one by one
                            // but android telnet send line of text (few chars together)
                            // and Telnet sometimes added some extra data (e.g. zeros or none printable characters)
                            // that we don't want to add to the message the none printable characters.
                            // First: accumulate all the typed characters into the buffer.
                            // Second: scan and insert only printable characters into the msgs.
                            
                            b_end_found = 0;
                            
                            for (int x=0; x<valread; x++)
                            {
                                if ( (client_cr[i] == 0) && (buffer[x] == 13) )
                                {
                                    b_end_found = 1;
                                    break;
                                }//End if ( (client_cr[i] == 0) && (buffer[x] == 13) )
									
								
                                if (client_cr[i] == 1)
                                {
                                    msgs[i][msgs_last_pos[i]] = buffer[x];
                                    msgs_last_pos[i]++;
                                }//End if (client_cr[i] == 1)
                                else if ((buffer[x]>=32)&&(buffer[x]<=127))
                                {
                                    // if (client_cr[i] == 0) take only printable characters
									msgs[i][msgs_last_pos[i]] = buffer[x];
                                    msgs_last_pos[i]++;
                                }//End 
                            }//End for (int x=0; x<valread; x+=)
								
                            
                            // Search if CR arrived (Carriage Return value is 0x0d) sign us for end-of-message
                            if ((buffer[0] == 0x0d) ||(b_end_found == 1))
                            { 
                                
                                // Since this is end-of-message we will look into it to see which command arrived.
                                // Check if this message is:
                                // 1. HELP:
                                // 2. LIST:            (Show registered client name in a list)
                                // 3. ECHO:on/off 
                                // 4. BYE:
                                // 5. EXIT:
                                // 6. ALL:message      (Broadcast to all registered clients)
                                // 7. NAME:new_name    (Registration)
                                // 8. name:message     (Send message to other registered client)
                                // 9. Parse Error 
                                
                                
                                //////////////////
                                // Handle HELP: //
                                //////////////////
                                if (CompareIgnoreCase(msgs[i], "HELP:") == 0)
                                {
                                    help(fds[i].fd, name[i], client_echo[i], client_cr[i]);
                                }// End if (CompareIgnoreCase(msgs[i], "HELP:") == 0)
                                
                                
                                //////////////////
                                // Handle LIST: //
                                //////////////////
                                else if (CompareIgnoreCase(msgs[i], "LIST:") == 0)
                                {
                                    list_of_clients(sd, fds, i, name);	
                                }//Endif (CompareIgnoreCase(msgs[i], "LIST:") == 0)
                                
                                
                                ////////////////////////
                                // Handle ECHO:on/off //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "ECHO:") == 0)
                                {
                                    int ln;
                                    ln = strlen(msgs[i]);
                                    char on_off[3];
                                    //printf("ECHO: command length=%d\n", ln);
                                    if (ln >= 7)
                                    {
                                        //Extract the "on"/"off" parameter from the message.
                                        for (int r=0; r<(ln-5); r++)
                                        {
                                            on_off[r] = msgs[i][r+5];
                                        }//End for (int r=0; r<(ln-5); r++)
                                        
                                        if (CompareIgnoreCase(on_off, "on") == 0)
                                        {
                                            client_echo[i] = 1;
                                        }
                                        
                                        if (CompareIgnoreCase(on_off, "off") == 0)
                                        {
                                            client_echo[i] = 0;
                                        }
                                    }
                                }//End else if if (CompareIgnoreCase(msgs[i], "ECHO:") == 0)
                                
                                
                                ////////////////////////
                                // Handle BYE:        //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "BYE:") == 0)
                                {
                                    client_bye(msgs_last_pos, i, name, msgs, sd);
                                }//End else if (CompareIgnoreCase(msgs[i], "BYE:") == 0)							
                                
                                
                                ////////////////////////
                                // Handle EXIT:       //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "EXIT:") == 0)
                                {
                                    client_exit(fds, msgs_last_pos, i, name, msgs, sd);
                                }//End else if (CompareIgnoreCase(msgs[i], "EXIT:") == 0)		
                                
                                
                                ////////////////////////
                                // Handle ERASE:       //
                                ////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "ERASE:") == 0)
                                {
                                    printf("Client name(%s) socket(%d) ERASE the list (everybody need to NAME again!!).\n", name[i], sd);
                                    
                                    sprintf(format_msg, "\r\n Client name(%s) socket(%d) ERASE the list (everybody need to NAME again!!).\r\n",name[i], sd);
                                    
                                    for(int m=0; m < MAX_CLIENTS_NUM; m++)
                                    {
                                        send(fds[m].fd, format_msg, strlen(format_msg), 0);
                                        // Clear the name & msgs.
                                        memset(msgs[m], 0x00, MAX_MSG_LEN);
                                        memset(name[m], 0x00, MAX_NAME_LEN);
                                        msgs_last_pos[m] = 0; 
                                    }//End for(int m=0; m < MAX_CLIENTS_NUM; m++)
                                }//End else if (CompareIgnoreCase(msgs[i], "ERASE:") == 0)
                                
                                
                                ////////////////////////
                                // Handle ALL:message //
                                ////////////////////////
                                // If four first letter are "ALL:" broadcast the message to All the clients.
                                else if (CompareIgnoreCase(msgs[i], "ALL:") == 0)
                                {
                                    broadcast_all(sd, fds, i, name, msgs[i], client_echo[i]);
                                }//End if (CompareIgnoreCase(msgs[i], "ALL:") == 0)
                                
                                
                                ////////////////////////////
                                // Handle NAME:new_name   //
                                ////////////////////////////
                                else if (CompareIgnoreCase(msgs[i], "NAME:") == 0)	
                                {
                                    set_new_name(fds, i, name, msgs, client_echo[i]);
                                }//End if (CompareIgnoreCase(msgs[i], "NAME:") == 0)
                                else
                                {
                                    ///////////////////////////
                                    // Handle name:message   //
                                    ///////////////////////////
                                    int send_ret = 0;
                                    send_ret = send_message(fds, i, name, msgs, client_echo[i]);
                                    ///////////////////////////////////////
                                    // Handle send message Parse Error   //
                                    ///////////////////////////////////////
                                    // Name not found missing separator or no name was specify (no "NAME:" string).
                                    if (send_ret != 0)
                                    {
                                        // Wrong command  (did not find the name or no name specified).
                                        // Note: Sometimes using back key during editing are not handle properly, so e.g. then find name will fail.
                                        msgs[i][msgs_last_pos[i]-1] = 0x00;
                                        
                                        printf("Receive message: %s, from socket(%d) name(%s), could not parse correctly.\n",
                                               msgs[i], fds[i].fd, name[i]);
                                        send(fds[i].fd, fail_string, strlen(fail_string), 0);
                                    }//End  if (send_ret != 0)
                                    
                                }//End else if (CompareIgnoreCase(msgs[i], "NAME:") == 0)
                                
                                // After performing the command, clean the accumulate storage place, 
                                // to enable fresh start for the next command from this client.
                                memset(msgs[i], 0x00, MAX_MSG_LEN);
                                msgs_last_pos[i] = 0;
                            }//End if ((buffer[0] == 0x0d) ||(b_end_found == 1))
                            
                        }//End if ((strlen(msgs[i])+valread) < MAX_MSG_LEN)
                        else
                        {
                            // TODO:
                            // Handle msgs with bigger length than the buffer can hold ...
                            printf("Error: Can not handle msgs with accumulate bigger length than the buffer can hold.\n");
                           send(fds[i].fd, err_buff_acum, strlen(err_buff_acum), 0);
                        }//End else if ((strlen(msgs[i])+1) < MAX_MSG_LEN)
                    }//End else of "else if (valread == MAX_MSG_LEN)"
                }//End if (FD_ISSET(sd, &readfds))
            }//End for (i = 0; i < MAX_CLIENTS_NUM; i++)
        }//End else (activity != 0 and activity != -1)
        /*************************************************
        * USE POLL API - END
        **************************************************/
#endif /* USE SELECT */
    
    }//End while(TRUE)
    return 0;
}//End main()
