#include "echoclientcontext.h"
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define USERNAME    1
#define HOSTNAME    2
#define PORT        3

static void *read_thread( void *arg );
FILE *logfile;

int main( int argc, char *argv[ ] )
{
    char buffer[ 256 ], filename[ 256 ];
    echo_client_context_t *client;
    tcp_context_t *ctx;
    pthread_t thread;
    int err;

    if( argc != 4 )
    {
        fprintf( stderr, "USAGE: %s USERNAME HOSTNAME PORT\n", argv[ 0 ] );
        return EXIT_FAILURE;
    }

    strcpy( filename, argv[ USERNAME ] );
    strcat( filename, ".log" );

    if( ( logfile = fopen( filename, "a+" ) ) == NULL )
    {
        perror( "fopen" );
        return EXIT_FAILURE;
    }

    if( ( ctx = tcp_context_create( &err ) ) == NULL )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "tcp_context_create: %s.\n", buf );
        return EXIT_FAILURE;
    }

    if( tcp_context_connect( ctx, argv[ HOSTNAME ],
                atoi( argv[ PORT ] ), &err ) == -1 )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "tcp_context_connect: %s.\n", buf );
        return EXIT_FAILURE;
    }

    tcp_context_send( ctx, argv[ USERNAME ], strlen( argv[ USERNAME ] ),
            &err );

    if( tcp_context_recv( ctx, buffer, 256, &err ) > 0 )
    {
        if( strcmp( buffer, "FAILED" ) == 0 )
        {
            printf( "Username already taken\n" );
            return EXIT_FAILURE;
        }
    }

    client = echo_client_context_create( ctx, argv[ USERNAME ], &err );

    if( client == NULL )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "echo_client_context_create: %s.\n", buf );
        tcp_context_destroy( ctx );
        return EXIT_FAILURE;
    }

    err = pthread_create( &thread, NULL, read_thread, client );

    if( err != 0 )
    {
        perror( "pthread_create" );
        echo_client_context_destroy( client );
        return EXIT_FAILURE;
    }

    while( !feof( stdin ) )
    {
        printf( "%s> ", argv[ USERNAME ] );

        fgets( buffer, 256, stdin );
        buffer[ strlen( buffer )-1 ] = '\0';

        if( !feof( stdin ) )
        {
            tcp_context_send( ctx, buffer, strlen( buffer ), &err );
        }
    }

    echo_client_context_destroy( client );
    fclose( logfile );

    return EXIT_SUCCESS;
}

void *read_thread( void *arg )
{
    echo_client_context_t *client;
    char buffer[ 4096 ];
    ssize_t bytes;
    int err;

    pthread_detach( pthread_self( ) );
    client = ( echo_client_context_t* )arg;

    while( ( bytes = tcp_context_recv( client->eec_tcp, buffer, 4096,
                    &err ) ) > 0 )
    {
        fputs( buffer, logfile );
        fflush( logfile );
        memset( buffer, 0, 4096 );
    }

    return NULL;
}
