This command line chat application implement "client"" and "server" 
write in c with socket API "select" (or can compile to use socket API "poll")
first run the server "server.exe" then run the clients "client.exe".

In the example below use PowerShell three command lines a 
server, client(1)"avi, client(2)"ben" which send message "Hello" to client(1)"avi".

CHAT_SERVER
~~~~~~~~~~~
C:\MyData\Server_Client\Server> .\server.exe
Welcome to Chat Server v1.4
The Server is using select() method
Listener on port 8888
Waiting for connections ...



CHAT_CLIENT_1
~~~~~~~~~~~~~
PC:\MyData\Server_Client\Client> .\client.exe
Socket created
Connected
Welcome to Chat Server v1.4
name:avi
Name Added.


CHAT_CLIENT_2
~~~~~~~~~~~~~
C:\MyData\Server_Client\Client> .\client.exe
Socket created
Connected
Welcome to Chat Server v1.4
name:ben
Name Added.

list:
1) name=avi socket=4
2) name=ben socket=5 (me)

avi:Hello
Send to Client.

CHAT_CLIENT_1
~~~~~~~~~~~~~
ben->avi:Hello


CHAT_SERVER
~~~~~~~~~~~
New connection, socket:4, ip:127.0.0.1, port:55325
Welcome message sent successfully
Adding to list of sockets at position:0 socket:4.
Socket(4) add new name=avi
New connection, socket:5, ip:127.0.0.1, port:55339
Welcome message sent successfully
Adding to list of sockets at position:1 socket:5.
Socket(5) add new name=ben
Client socket(5) name(ben) request LIST:
1) name=avi socket=4
2) name=ben socket=5 (me)
Message to Cilent: ben->avi:Hello


CHAT_CLIENT_1
~~~~~~~~~~~~~
help:


NAME:new_name
Add the <new_name> to server list,
so registered clients will be able
to send messages to specific client
by using its <new_name>.

name:message
Will send the <message> to specific client <name>.

LIST:
Will show a list of all registered clients.

ALL:message
The <message> will broadcast to all registered clients.

ECHO:off/on
off (0) (default) server will not send back to client received chars as feedback.
on  (1)           server send back to client received chars as feedback.

CR:off/on
off (0)(default) dont show non printable chars, skip <CR><LF>.
on  (1)          show all recevied characters include <CR><LF>.

BYE:
Leave without notice to anyone.

EXIT:
Inform every one you are going to leave, then leave.

ERASE:
Erase the registered clients list
(everybody need to NAME again!!)


User_Params:
------------
name=avi
socket=4
echo=0
cr=0





For Client compilation:
run "Cygwin64 Terminal" get the "$" prompt:
$ cd C:\\MyData\\Server_Client\\Client
$ gcc -o client client.c
copy "cygwin1.dll" from "C:\cygwin64\bin" to the folder with the compiled "client.exe".
 
 
For the server compilation:
#define USE_SELECT //Allow the define above definition will cause to work with socket "select()" function (the old way) 
Undefined above definition will cause to work with socket "poll()" function (instead of "select()")

run "Cygwin64 Terminal" get the "$" prompt:
$ cd C:\\MyData\\Server_Client\\Server
make clean
make 
copy "cygwin1.dll" from "C:\cygwin64\bin" to the folder with the compiled "server.exe".

NOTE:
~~~~~
for example mixing 32bit "cygwin1.dll" with "server.exe" compile with cygwin 64 bit will crash.
with runtime meaasge box display:
Application Error
The application was unable to start correctly(0xc000007b).
click OK to close the application

