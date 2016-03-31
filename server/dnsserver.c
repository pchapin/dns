/****************************************************************************
FILE   : dnsserver.c
SUBJECT: Implementation of a simple DNS server.

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#ifndef S_SPLINT_S    // Workaround for splint.
#include <unistd.h>
#endif


int main(int argc, char **argv)
{
    // TODO: Implement me!

    printf("Hello, World!\n");
    return EXIT_SUCCESS;
}
