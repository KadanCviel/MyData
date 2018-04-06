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





/*
  File name: Client_chat.c 
  
  By modify the "C ECHO client example using sockets"
  open new child process by using "fork" (after the connect)
  The child use "gets" for collect user input charcter and use socket Tx (write)
  to transmit the data as a line of text to Sever_chat
  ending with 0x0d (decimal 13) as indicatore for EOL (End-Of-Line).
  The parent process using socket Rx (read) to data from Server_chat,

Original: C ECHO client example using sockets 
http://www.binarytides.com/server-client-example-c-sockets-linux/

https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1044652485&id=1043284385



for compilation:
----------------
cd C:\\MyData\\Server_Client\\Client
gcc -o client client.c


for runninng:
-------------
cmd
cd c:\MyData\Server_Client\Client
client.exe


*/


#include <stdio.h> //printf
#include <stdlib.h>
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>


#define MAXLINE  255
#define BUFF_SIZE 256
#define SHORT_DELAY 10
#define PORT 8888
#define MIN_PORT 2000
#define MAX_PORT 65535
#define MAX_BUFF_SIZE 8192

void help(void)
{
    printf("\nClient_chat.exe IP_ADDR PORT_NUM\n\n");
    printf("Client_chat.exe PORT_NUM   (Use default IP_ADDR 127.0.0.1)\n");
    printf("Client_chat.exe            (Use default IP_ADDR 127.0.0.1 and default PORT_NUM 8888)\n");
    printf("Client_chat.exe h/H/help   (This help)\n");
    printf("Note: PORT_NUM should be in range[%d,%d]\n", MIN_PORT, MAX_PORT);
}//End help()


int main(int argc , char *argv[])
{
    int sock, pid, port_num;
    struct sockaddr_in server;

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    
    if (argc > 2)
    {
        //we will get IP & PORT as arguments
        server.sin_addr.s_addr = inet_addr(argv[1]);
        port_num = PORT;
        port_num = atoi(argv[2]);
        if ( (port_num < MIN_PORT) || (port_num > MAX_PORT) )
        {
           printf("Error: PORT_NUM not in range[%d,%d]\n", MIN_PORT, MAX_PORT);
           return 0;
        }
    }//End if (argc > 2)
    else if (argc == 2)
    {
        if ((strncmp(argv[1], "h", 1) == 0) ||
            (strncmp(argv[1], "H", 1) == 0) ||
            (strncmp(argv[1], "help", 4) == 0))
        {
            help();
            return 0;
        }
        else        
        {
            server.sin_addr.s_addr = inet_addr("127.0.0.1");
            port_num = PORT;
            port_num = atoi(argv[1]);
            if ( (port_num < MIN_PORT) || (port_num > MAX_PORT) )
            {
                printf("Error: PORT_NUM not in range[%d,%d]\n", MIN_PORT, MAX_PORT);
                return 0;                
            }
        }//End if ( (port_num < MIN_PORT) || (port_num > MAX_PORT) )
    }//End if ( argc == 2 )
    else
    {
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        port_num = PORT ;  //e.g. 8888 

    }//End else if (argc == 2)
    
    server.sin_port = htons(port_num);  //e.g. default 8888    
    
    
    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    
    
    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        //perror("connect failed. Error");
        printf("Error: Connect() failed. Check if Server is running or check arguments (Client_chat.exe help)\n");
        return 1;
    }
     
    //puts("Connected\n");
    printf("Connected\n");
    
    /* Create child process */
    pid = fork();
    
    if (pid < 0) 
    {
        //perror("ERROR on fork");
        printf("Error: fork() failed.\n");
        exit(1);
    }
      
    if (pid == 0) 
    {
        /* This is the child process */
        // Child use "fgets" to receive user input characters string termiante with "\n". 
	
        int n;
        char buffer[BUFF_SIZE];
        char tav[2];
        int new_len;			
		char send_buffer[MAX_BUFF_SIZE];
		int pos;
		bzero(send_buffer, MAX_BUFF_SIZE);
		
        while(1)
        {
            bzero(buffer, BUFF_SIZE);
            fgets(buffer, MAXLINE, stdin);
            /* printf("\n"); */
            new_len = strlen(buffer);
			if (new_len == 1)
			{
				//press on empty line - trigger sending the accumulate buffer
				pos = strlen(send_buffer);
				
				
				// NEW special handler for file transfer
				
//                if (CompareIgnoreCase(msgs[i], "FILE:") == 0)
//                {
					//we found that the command if "FILE" transfer
					// format is "FILE:NAME:PATH\FILE_NAME.EXT"
					//NAME is the client that the file will send to him.
					//PATH is the local computer path were the file located.
					//FILE_NAME the file name with extension in the PATH the we want to send.
					//The file name in the received client will be made from "NAME" of the sender & "YYYY-MM-DD-HH-MM-SS" and the "FILE_NAME.EXT"
					//and will store in the current directory where the receiver client is running.
					//TBD: server will store copy of the file.
					//indication will send to the client informing him for the file he received.
					//When sending complete send indicatin to sender(printf).
					//the file will split to chunks of 500 bytes, each chunk will indicate the file name to create/append, length of the chunk.
					// e.g NAME" of the sender & "YYYY-MM-DD-HH-MM-SS" and the "FILE_NAME.EXT"
					
//				}
//				else
//				{
					*(send_buffer + pos) = 0x0d;
					
					n = write(sock, send_buffer, strlen(send_buffer));
					if (n < 0)
					{
						//perror("ERROR writing to socket");
						printf("Error: Write 1 to socket failed, check if Server is running.\n");
						exit(0);
					}//End if (n < 0)
					bzero(send_buffer, MAX_BUFF_SIZE);					
					/* printf("send to socket(%d).\n",sock); */
//				}
			}
			else
			{
				pos = strlen(send_buffer);
				memcpy(send_buffer+pos, buffer, new_len);
			}
        }//End while(1)
            
    }//End if (pid == 0) 
    else 
    {
        /* This is the parent process */
        // The parent process wait in read() state until data arrived.
        // on data arrival it print the income data, then go back to read() state.
        char r_buffer[BUFF_SIZE];
        int m;
        
        while(1)
        {
            bzero(r_buffer, BUFF_SIZE);
            m = read(sock, r_buffer, MAXLINE);
            if (m < 0)
            {
                //perror("ERROR reading from socket");
                printf("Error: Reading from socket failed, check if Server is running.\n");
                exit(0);
            }//End if (m < 0)
            printf("%s",r_buffer);
        }//End while(1)
        
    }//End else if (pid == 0) 
}//End main()