#include "echoservercontext.h"
#include <errno.h>

static int context_cmp( const void *a, const void *b );
static int uname_cmp( const void *a, const void *b );

void echo_server_context_perror( const char *s )
{
    if( echo_server_context_errno == EDUPLICATE )
    {
        printf( "%s: Duplicate context insertion.\n", s );
    }
    else
    {
        errno = echo_server_context_errno;
        perror( s );
    }
}

echo_server_context_t *echo_server_context_create( tcp_context_t *ctx )
{
    echo_server_context_t *server;

    if( ctx == NULL )
    {
        echo_server_context_errno = EINVAL;
        return NULL;
    }

    if( ( server = malloc( sizeof( echo_server_context_t ) ) ) == NULL )
    {
        echo_server_context_errno = ENOMEM;
        return NULL;
    }

    server->esc_bag = bag_array_create( );

    if( server->esc_bag == NULL )
    {
        free( server );
        return NULL;
    }

    server->esc_tcp = ctx;

    return server;
}

int echo_server_context_insert( echo_server_context_t *ctx,
        echo_client_context_t *client )
{
    ssize_t found;

    if( ctx == NULL || client == NULL )
    {
        echo_server_context_errno = EINVAL;
        return -1;
    }

    found = 0;
    found = bag_array_find_first( ctx->esc_bag, client, &found, uname_cmp );

    if( found != -1 )
    {
        echo_server_context_errno = EDUPLICATE;
        return -1;
    }

    return bag_array_insert( ctx->esc_bag, client );
}

tcp_context_t *echo_server_context_remove( echo_server_context_t *ctx,
        echo_client_context_t *client )
{
    ssize_t found;

    if( ctx == NULL || client == NULL )
    {
        echo_server_context_errno = EINVAL;
        return NULL;
    }

    found = 0;
    found = bag_array_find_first( ctx->esc_bag, client, &found, context_cmp );

    if( found == -1 )
    {
        return NULL;
    }

    return bag_array_remove( ctx->esc_bag, found );
}

void echo_server_context_sendall( echo_server_context_t *ctx,
        const char *buffer, size_t size )
{
    echo_client_context_t *client;
    size_t i;

    if( ctx != NULL && buffer != NULL && size > 0 )
    {
        for( i = 0; i < ctx->esc_bag->b_size; i++ )
        {
            client = bag_array_get( ctx->esc_bag, i );
            tcp_context_send( client->eec_tcp, buffer, size );
        }
    }
}

void echo_server_context_destroy( echo_server_context_t *ctx )
{
    tcp_context_destroy( ctx->esc_tcp );
    bag_array_destroy( ctx->esc_bag );
    free( ctx );
}

int context_cmp( const void *a, const void *b )
{
    return !( a == b );
}

int uname_cmp( const void *a, const void *b )
{
    echo_client_context_t *eec1, *eec2;

    eec1 = ( echo_client_context_t* )a;
    eec2 = ( echo_client_context_t* )b;

    return strcmp( eec1->eec_uname, eec2->eec_uname );
}
