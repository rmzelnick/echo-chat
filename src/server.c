#include "echoservercontext.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define BACKLOG 1000

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
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
    int err;

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

    if( ( ctx = tcp_context_create( &err ) ) == NULL )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "tcp_context_create: %s.\n", buf );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile,
            "DONE\nBinding server's tcp context to localhost... " );

    if( tcp_context_bind( ctx, atoi( argv[ 1 ] ), &err ) == -1 )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "tcp_context_bind: %s.\n", buf );
        tcp_context_destroy( ctx );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "DONE\nListening for incoming connections... " );

    if( tcp_context_listen( ctx, BACKLOG, &err ) == -1 )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "tcp_context_listen: %s.\n", buf );
        tcp_context_destroy( ctx );
        fclose( logfile );
        return EXIT_FAILURE;
    }

    fprintf( logfile, "DONE\nCreating echo server context... " );
    server = echo_server_context_create( ctx, &err );

    if( server == NULL )
    {
        char buf[ 256 ];
        tcp_context_strerror( err, buf, 256 );
        fprintf( stderr, "echo_server_context_create: %s.\n", buf );
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

    pthread_mutex_lock( &g_lock );

    for( i = 0; i < server->esc_bag->b_size; i++ )
    {
        echo_client_context_destroy( bag_array_get( server->esc_bag, i,
                    &err ) );
    }

    echo_server_context_destroy( server );
    pthread_mutex_unlock( &g_lock );

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
    int err;

    pthread_detach( pthread_self( ) );
    server = ( echo_server_context_t* )arg;

    while( 1 )
    {
        ctx = tcp_context_accept( server->esc_tcp, &err );

        fprintf( logfile, "Accepting incoming connection... " );

        if( ctx != NULL )
        {
            memset( username, 0, MAX_LENGTH );
            tcp_context_recv( ctx, username, MAX_LENGTH, &err );
            client = echo_client_context_create( ctx, username, &err );

            if( client != NULL )
            {
                int tmp;

                pthread_mutex_lock( &g_lock );
                args.a_server = server;
                args.a_client = client;
                tmp = echo_server_context_insert( server, client, &err );
                pthread_mutex_unlock( &g_lock );

                if( tmp == -1 )
                {
                    if( err == EDUPLICATE )
                    {
                        tcp_context_send( client->eec_tcp, "FAILED", 7,
                                &err );
                    }

                    echo_client_context_destroy( client );
                    fprintf( logfile, "FAILED\n" );
                    continue;
                }

                tcp_context_send( client->eec_tcp, "DONE", 5, &err );
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
    int err;

    pthread_mutex_lock( &g_lock );
    args = ( struct argument* )arg;
    server = args->a_server;
    client = args->a_client;
    pthread_mutex_unlock( &g_lock );

    sprintf( message, "%s joined\n", client->eec_uname );

    pthread_mutex_lock( &g_lock );
    echo_server_context_sendall( server, message, strlen( message ), &err );
    pthread_mutex_unlock( &g_lock );

    while( ( bytes = tcp_context_recv( client->eec_tcp,
                    buffer, 2048, &err ) ) > 0 )
    {
        sprintf( message, "%s says:\n%s\n", client->eec_uname, buffer );
        pthread_mutex_lock( &g_lock );
        echo_server_context_sendall( server, message, strlen( message ),
                &err );
        pthread_mutex_unlock( &g_lock );
        memset( buffer, 0, 2048 );
    }

    sprintf( message, "%s left\n", client->eec_uname );

    pthread_mutex_lock( &g_lock );

    echo_server_context_remove( server, client, &err );
    echo_server_context_sendall( server, message, strlen( message ), &err );
    pthread_mutex_unlock( &g_lock );

    echo_client_context_destroy( client );

    return NULL;
}
