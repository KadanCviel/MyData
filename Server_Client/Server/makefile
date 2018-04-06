#
# Copyright (c) 2018, Kadan. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   - Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#   - Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#   - Neither the name of Kadan or the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#
#
#
# ---------------------------------
# compilation using gcc on Cygwin64
# ---------------------------------
#
# run "Cygwin64 Terminal" get the "$" prompt:
#  $ cd C:\\MyData\\Server_Client\\Server
#  make
#  make clean
# copy to the compiled server.exe" the "cygwin1.dll" file
# from "C:\cygwin64\bin" cygwin istalled folder
# if you complile with another cyggwin e.g. 32 bit cygwin 
# it will crash if using dll of 64 bit cygwin. 
# with runtime meaasge box display:
# Application Error 
# The application was unable to start correctly(0xc000007b).
# click OK to close the application

server:  utils.o list.o broadcast.o new_name.o message.o misc.o main.o
	gcc -o server utils.o list.o broadcast.o new_name.o message.o misc.o main.o 
	
utils.o : utils.c
	gcc -Wall -g -c utils.c
	
list.o : list.c
	gcc -Wall -g -c list.c

broadcast.o : broadcast.c
	gcc -Wall -g -c broadcast.c

new_name.o : new_name.c
	gcc -Wall -g -c new_name.c

message.o : message.c
	gcc -Wall -g -c message.c

misc.o : misc.c
	gcc -Wall -g -c misc.c
	
main.o : main.c
	gcc -Wall -g -c main.c
	
clean:
	rm -f server utils.o list.o broadcast.o new_name.o message.o misc.o main.o 