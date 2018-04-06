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
//  File       : utils.c
//  Description: Chat Server 
//  Name       : Cviel Kadan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "header.h"


/////////////////////////////////////////////////////////////
// Function: prep_msg
// Description: format string for sending
/////////////////////////////////////////////////////////////
void prep_msg(char* from, char* in_msg, char* out_msg)
{
    sprintf(out_msg, "\r\n%s->%s\r\n", from, in_msg);
}//End  prep_msg()



/////////////////////////////////////////////////////////////
// Function: CompareIgnoreCase
// Description: Compare input string (s1) with received expected string (s2) Ignore CaseSensative
//              for the first letters from s2
//              0 - are equal
//              1 - not equal
//              2 - first string is too short
/////////////////////////////////////////////////////////////
int CompareIgnoreCase(char* s1, char* s2)
{
    int i;
   
    if (strlen(s1) < strlen(s2))
    {
       // input string (s1) it too short, no point to continue
       return 2;
    }
    
    for(i=0; i<strlen(s2); i++)
    {
        if ( tolower(s1[i]) != tolower(s2[i]) )
        {
            //if one char is not match then we can exit and report not match
            return 1;
        }//End if ( tolower(s1[i]) != tolower(s2[i]) )
    }//End for(i=0; i<strlen(s2); i++)
    
    // found a matching strings (ignore thecase sensative)
    return 0;
}//End CompareIgnoreCase()



