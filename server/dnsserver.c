/****************************************************************************
FILE   : dnsserver.c
SUBJECT: Implementation of a simple DNS server.

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Williston, VT 05495
     pchapin@vtc.edu
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#ifndef S_SPLINT_S    // Workaround for splint.
#include <unistd.h>
#endif

#define DATAGRAM_BUFFER_SIZE 512

// These buffers hold the datagrams used by the server.
unsigned char query[DATAGRAM_BUFFER_SIZE];
unsigned char reply[DATAGRAM_BUFFER_SIZE];


int main( int argc, char **argv )
{
    // +++ Set up the UDP socket

    while( 1 ) {
        // +++ Receive a request from a client.

        // +++ Decode request. Is it valid? If not, ignore it.
        //     Print message to standard output:
        //     - IP address and port number of client.
        //     - If the request is valid or not
        //     - For valid requests, the domain name(s) requested.

        // +++ Construct reply. Use an IP address of 127.0.0.1 for all domain names
        //     FUTURE ENHANCEMENT: Look up IP addresses from a database.

        // +++ Send reply to client.
    }

    printf( "Hello, World!\n" );
    return EXIT_SUCCESS;
}
