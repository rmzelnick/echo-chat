#include "tcpcontext.h"
#include <pthread.h>
#include <errno.h>

static pthread_mutex_t g_errno_lock = PTHREAD_MUTEX_INITIALIZER;

void tcp_context_strerror( int errnum, char *buf, size_t buflen )
{
    strerror_r( errnum, buf, buflen );
}

tcp_context_t *tcp_context_create( int *err )
{
    tcp_context_t *ctx;

    if( ( ctx = malloc( sizeof( tcp_context_t ) ) ) == NULL )
    {
        *err = ENOMEM;
        return NULL;
    }

    ctx->tc_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if( ctx->tc_socket == -1 )
    {
        free( ctx );
        return NULL;
    }

    return ctx;
}

int tcp_context_connect( tcp_context_t *ctx, const char *host, int port,
       int *err )
{
    struct hostent *ent;

    if( ctx == NULL || host == NULL || port > 65535 )
    {
        *err = EINVAL;
        return -1;
    }

    if( ( ent = gethostbyname( host ) ) == NULL )
    {
        pthread_mutex_lock( &g_errno_lock );
        *err = h_errno;
        pthread_mutex_unlock( &g_errno_lock );
        return -1;
    }

    memset( &ctx->tc_addr, 0, sizeof( ctx->tc_addr ) );
    memcpy( &ctx->tc_addr.sin_addr, ent->h_addr_list[ 0 ], ent->h_length );
    ctx->tc_addr.sin_family = AF_INET;
    ctx->tc_addr.sin_port = htons( port );

    return connect( ctx->tc_socket, ( struct sockaddr* )&ctx->tc_addr,
            sizeof( ctx->tc_addr ) );
}

int tcp_context_bind( tcp_context_t *ctx, int port, int *err )
{
    int retval, optval;

    if( ctx == NULL || port > 65535 )
    {
        *err = EINVAL;
        return -1;
    }

    optval = 1;
    setsockopt( ctx->tc_socket, SOL_SOCKET, SO_REUSEADDR, &optval,
            sizeof( optval ) );

    memset( &ctx->tc_addr, 0, sizeof( ctx->tc_addr ) );
    ctx->tc_addr.sin_addr.s_addr = INADDR_ANY;
    ctx->tc_addr.sin_family = AF_INET;
    ctx->tc_addr.sin_port = htons( port );

    if( ( retval = bind( ctx->tc_socket, ( struct sockaddr* )&ctx->tc_addr,
                sizeof( ctx->tc_addr ) ) ) == -1 )
    {
        pthread_mutex_lock( &g_errno_lock );
        *err = errno;
        pthread_mutex_unlock( &g_errno_lock );
    }

    return retval;
}

int tcp_context_listen( tcp_context_t *ctx, int backlog, int *err )
{
    int retval;

    if( ctx == NULL || backlog <= 0 )
    {
        *err = EINVAL;
        return -1;
    }

    if( ( retval = listen( ctx->tc_socket, backlog ) ) == -1 )
    {
        pthread_mutex_lock( &g_errno_lock );
        *err = errno;
        pthread_mutex_unlock( &g_errno_lock );
    }

    return retval;
}

tcp_context_t *tcp_context_accept( const tcp_context_t *ctx, int *err )
{
    tcp_context_t *client;
    socklen_t size;

    if( ctx == NULL )
    {
        *err = EINVAL;
        return NULL;
    }

    if( ( client = malloc( sizeof( tcp_context_t ) ) ) == NULL )
    {
        *err = ENOMEM;
        return NULL;
    }

    size = sizeof( client->tc_addr );
    client->tc_socket = accept( ctx->tc_socket,
            ( struct sockaddr* )&client->tc_addr, &size );

    if( client->tc_socket == -1 )
    {
        pthread_mutex_lock( &g_errno_lock );
        *err = errno;
        pthread_mutex_unlock( &g_errno_lock );
        free( client );
        return NULL;
    }

    return client;
}

ssize_t tcp_context_send( tcp_context_t *ctx, const char *buffer,
        size_t size, int *err )
{
    int retval;

    if( ctx == NULL || buffer == NULL || size == 0 )
    {
        *err = EINVAL;
        return -1;
    }

    if( ( retval = send( ctx->tc_socket, buffer, size, 0 ) ) == -1 )
    {
        pthread_mutex_lock( &g_errno_lock );
        *err = errno;
        pthread_mutex_unlock( &g_errno_lock );
    }

    return retval;
}

ssize_t tcp_context_recv( tcp_context_t *ctx, char *buffer, size_t size,
       int *err )
{
    int retval;

    if( ctx == NULL || buffer == NULL || size == 0 )
    {
        *err = EINVAL;
        return -1;
    }

    if( ( retval = recv( ctx->tc_socket, buffer, size, 0 ) ) == -1 )
    {
        pthread_mutex_lock( &g_errno_lock );
        *err = errno;
        pthread_mutex_unlock( &g_errno_lock );
    }

    return retval;
}

void tcp_context_destroy( tcp_context_t *ctx )
{
    shutdown( ctx->tc_socket, SHUT_RDWR );
    close( ctx->tc_socket );
    free( ctx );
}
