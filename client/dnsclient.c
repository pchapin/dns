/****************************************************************************
FILE   : dnsclient.c
SUBJECT: Implementation of a simple DNS client.

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Williston, VT 05495
     pchapin@vtc.edu
****************************************************************************/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#ifndef S_SPLINT_S    // Workaround for splint.
#include <unistd.h>
#endif

#define WORKSPACE_SIZE 512
#define NAME_SIZE      16

void SIGALRM_handler( int signal_number )
{
    // Do nothing. Deal with the problem when recvfrom() returns with EINTR.
    // We can't, in theory, use printf() or similar functions from inside a signal handler.
    return;
}


//! Sets up the handling of the SIGALRM signal.
/*!
 * This function arranges to have SIGALRM_handler() called when a SIGALRM signal occurs. This
 * happens when recvfrom() takes too long to return.
 */
void initialize_signal_handling( )
{
    struct sigaction action;

    action.sa_handler = SIGALRM_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGALRM, &action, NULL);
}


//! Copies the given domain name into the workspace, adding label lengths in the process.
/*!
 * \param p Pointer to the place in the workspace where the domain name will go.
 * \param domain_name Pointer to the name itself in human readible form.
 * \return A pointer just past the end of the installed domain name (including the null byte).
 */
unsigned char *install_domain_name(unsigned char *p, char *domain_name)
{
    // .lemuria.cis.vtc.edu\0
    *p++ = '.';
    strcpy((char *)p, domain_name);
    p--;

    while (*p != '\0') {
        if (*p == '.') {
            unsigned char *end = p + 1;
            while (*end != '.' && *end != '\0') end++;
            *p = end - p - 1;
        }
        p++;
    }
    return p + 1;
}


int main(int argc, char **argv)
{
    int            socket_handle;
    int            rc;              // Used to hold return codes from network functions.
    unsigned short port = 53;                  // Default DNS port number.
    unsigned char  workspace[WORKSPACE_SIZE];  // Used to hold datagrams.
    char           name_buffer[NAME_SIZE];     // Used to hold the textual IP address.
    unsigned char *p;                          // Points into the workspace array.
    socklen_t      address_length;
    struct sockaddr_in server_address;

    // Do we have a command line arguments?
    if (argc != 3) {
        fprintf(stderr, "usage: %s server-ipaddress domain-name\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create a socket.
    if ((socket_handle = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Unable to create socket");
        return EXIT_FAILURE;
    }

    // 1. Construct the outgoing datagram...

    p = workspace;
    *p++ = 0x00; // MSB of ID.
    *p++ = 0x01; // LSB of ID.
    *p++ = 0x01; // QR = 0, Opcode = 0, AA = 0, TC = 0, RD = 1.
    *p++ = 0x00; // RA = 0, Z = 0, RCODE = 0
    *p++ = 0x00; // QDCOUNT = 1
    *p++ = 0x01;
    *p++ = 0x00; // ANCOUNT = 0
    *p++ = 0x00;
    *p++ = 0x00; // NSCOUNT = 0
    *p++ = 0x00;
    *p++ = 0x00; // ARCOUNT = 0
    *p++ = 0x00;
     p   = install_domain_name( p, argv[2] );
    *p++ = 0x00; // QTYPE  = 1
    *p++ = 0x01;
    *p++ = 0x00; // QCLASS = 1
    *p++ = 0x01;

    // 2. Use sendto() to send the datagram...

    // Fill in the address structure.
    memset( &server_address, 0, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons( port );
    if( inet_pton( AF_INET, argv[1], &server_address.sin_addr ) <= 0 ) {
        fprintf( stderr, "Unable to convert address into binary form\n" );
        close( socket_handle );
        return EXIT_FAILURE;
    }
    address_length = sizeof(server_address);

    rc = sendto(socket_handle,   // Socket to use.
                workspace,       // Pointer to outgoing datagram.
                p - workspace,   // Number of bytes to send.
                0,               // Flags.
                (struct sockaddr *)&server_address,  // Pointer to destination address.
                address_length); // Number of bytes in destination address.
    if( rc == -1 ) {
        perror( "Unable to send DNS request" );
        close( socket_handle );
        return EXIT_FAILURE;
    }

    // 3. Use recvfrom() to receive the response...

    memset( &server_address, 0, sizeof( server_address ) );
    address_length = sizeof( server_address );
    initialize_signal_handling( );
    alarm( 10 );  // Send SIGALRM in 10 seconds.
    rc = recvfrom(socket_handle,   // Socket to use. Same as sending.
                  workspace,       // Pointer to buffer for incoming datagram.
                  WORKSPACE_SIZE,  // Size of the buffer.
                  0,               // Flags.
                  (struct sockaddr *)&server_address,  // Pointer to source address.
                  &address_length);  // Pointer to variable to hold address size.
    if( rc == -1 ) {
        if( errno == EINTR ) {
            // This isn't perfect because in theory there are other reasons for EINTR.
            printf( "Timed out trying to receive DNS reply\n" );
        }
        else {
            perror( "Unable to receive DNS reply" );
        }
        close( socket_handle );
        return EXIT_FAILURE;
    }
    alarm( 0 );   // Cancel the alarm.

    // 4. Decode and display the answer...

    // This approach is a bit simplistic. Compression might be used for only part of the name.
    // We should also make sure the received datagram is large enough.
    //
    p  = workspace;
    p += 12;  // p now points past the header.
    p += 2 + strlen( argv[2] ) + 4;  // p now points at the answer.
    if( (*p & 0xC0) == 0xC0 ) {
        // Compression is being used.
        p += 12;
    }
    else {
        p += 2 + strlen(argv[2]) + 10;
    }
    // p now points at the address.

    if( inet_ntop( AF_INET, p, name_buffer, NAME_SIZE ) == NULL ) {
        perror( "Unable to convert the address to textual form" );
        close( socket_handle );
        return EXIT_FAILURE;
    }
    printf( "Host %s has IP address: %s\n", argv[2], name_buffer );

    // Close the socket to clean up.
    close(socket_handle);
    return EXIT_SUCCESS;
}
