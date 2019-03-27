#include "tcpcontext.h"
#include <string.h>
#include <assert.h>
#include <pthread.h>

#define PORT    5000
#define BACKLOG 1000

static void *connection_thread( void *arg );

int main( void )
{
    tcp_context_t *client_ctx, *server_ctx;
    char buffer[ 256 ];
    pthread_t thread;
    ssize_t bytes;

    assert( ( server_ctx = tcp_context_create( ) ) != NULL );
    assert( tcp_context_bind( server_ctx, PORT ) != -1 );
    assert( tcp_context_listen( server_ctx, BACKLOG ) != -1 );

    pthread_create( &thread, NULL, connection_thread, NULL );

    client_ctx = tcp_context_accept( server_ctx );
    assert( client_ctx != NULL );

    bytes = tcp_context_recv( client_ctx, buffer, 256 );

    assert( !strncmp( buffer, "hello, world", bytes ) );

    tcp_context_destroy( client_ctx );
    tcp_context_destroy( server_ctx );

    return EXIT_SUCCESS;
}

void *connection_thread( void *arg )
{
    tcp_context_t *client_ctx;
    char buffer[ ] = "hello, world";

    pthread_detach( pthread_self( ) );
    client_ctx = tcp_context_create( );

    tcp_context_connect( client_ctx, "localhost", PORT );
    tcp_context_send( client_ctx, buffer, strlen( buffer ) );

    return NULL;
}
