#include "echoservercontext.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define BACKLOG 1000

static void *accept_thread( void *arg );
static void *connex_thread( void *arg );
FILE *logfile;

struct argument
{
    echo_server_context_t *a_server;
    echo_client_context_t *a_client;
};

int main( int argc, char *argv[ ] )
{
    echo_server_context_t *server;
    tcp_context_t *ctx;
    pthread_t thread;
    size_t i;

    if( argc != 2 )
    {
        fprintf( stderr, "USAGE: %s PORT\n", argv[ 0 ] );
        return EXIT_FAILURE;
    }

    if( ( logfile = fopen( "server.log", "a+" ) ) == NULL )
    {
        perror( "fopen" );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "Creating server's tcp context... " );

    if( ( ctx = tcp_context_create( ) ) == NULL )
    {
        tcp_context_perror( "tcp_context_bind" );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile,
            "DONE\nBinding server's tcp context to localhost... " );

    if( tcp_context_bind( ctx, atoi( argv[ 1 ] ) ) == -1 )
    {
        tcp_context_perror( "tcp_context_bind" );
        tcp_context_destroy( ctx );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "DONE\nListening for incoming connections... " );

    if( tcp_context_listen( ctx, BACKLOG ) == -1 )
    {
        tcp_context_perror( "tcp_context_listen" );
        tcp_context_destroy( ctx );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "DONE\nCreating echo server context... " );
    server = echo_server_context_create( ctx );

    if( server == NULL )
    {
        echo_server_context_perror( "echo_server_context_create" );
        fprintf( logfile, "FAILED\n" );
        tcp_context_destroy( ctx );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "DONE\nSpawning acceptance thread... " );
    errno = pthread_create( &thread, NULL, accept_thread, server );

    if( errno != 0 )
    {
        perror( "pthread_create" );
        echo_server_context_destroy( server );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "DONE\n" );
    getchar( );

    for( i = 0; i < server->esc_bag->b_size; i++ )
    {
        echo_client_context_destroy( bag_array_get( server->esc_bag, i ) );
    }

    echo_server_context_destroy( server );
    fclose( logfile );

    return EXIT_SUCCESS;
}

void *accept_thread( void *arg )
{
    struct argument args;
    echo_server_context_t *server;
    echo_client_context_t *client;
    char username[ MAX_LENGTH ];
    tcp_context_t *ctx;
    pthread_t thread;

    pthread_detach( pthread_self( ) );
    server = ( echo_server_context_t* )arg;

    while( 1 )
    {
        ctx = tcp_context_accept( server->esc_tcp );
        fprintf( logfile, "Accepting incoming connection... " );

        if( ctx != NULL )
        {
            memset( username, 0, MAX_LENGTH );
            tcp_context_recv( ctx, username, MAX_LENGTH );
            client = echo_client_context_create( ctx, username );

            if( client != NULL )
            {
                args.a_server = server;
                args.a_client = client;

                if( echo_server_context_insert( server, client ) == -1 )
                {
                    if( echo_server_context_errno == EDUPLICATE )
                    {
                        tcp_context_send( client->eec_tcp, "FAILED", 7 );
                        echo_client_context_destroy( client );
                        fprintf( logfile, "FAILED\n" );
                        continue;
                    }
                    else
                    {
                        fprintf( logfile, "FAILED\n" );
                        echo_client_context_destroy( client );
                        continue;
                    }
                }

                tcp_context_send( client->eec_tcp, "DONE", 5 );
                pthread_create( &thread, NULL, connex_thread, &args );
                fprintf( logfile, "DONE\n" );
            }
            else
            {
                fprintf( logfile, "FAILED\n" );
            }
        }
        else
        {
            fprintf( logfile, "FAILED\n" );
        }
    }

    return NULL;
}

void *connex_thread( void *arg )
{
    struct argument *args;
    char buffer[ 2048 ], message[ 4096 ];
    echo_server_context_t *server;
    echo_client_context_t *client;
    ssize_t bytes;

    pthread_detach( pthread_self( ) );
    args = ( struct argument* )arg;
    server = args->a_server;
    client = args->a_client;

    sprintf( message, "%s joined\n", client->eec_uname );
    echo_server_context_sendall( server, message, strlen( message ) );

    while( ( bytes = tcp_context_recv( client->eec_tcp,
                    buffer, 2048 ) ) > 0 )
    {
        sprintf( message, "%s says:\n%s\n", client->eec_uname, buffer );
        echo_server_context_sendall( server, message, strlen( message ) );
        memset( buffer, 0, 2048 );
    }

    sprintf( message, "%s left\n", client->eec_uname );
    echo_server_context_sendall( server, message, strlen( message ) );

    echo_server_context_remove( server, client );
    echo_client_context_destroy( client );

    return NULL;
}
