#include <string.h>
#include <errno.h>
#include "echoclientcontext.h"

int echo_client_context_errno;

void echo_client_context_perror( const char *s )
{
    errno = echo_client_context_errno;
    perror( s );
}

echo_client_context_t *echo_client_context_create( tcp_context_t *ctx,
        const char *uname )
{
    echo_client_context_t *client;

    if( ctx == NULL || uname == NULL )
    {
        echo_client_context_errno = EINVAL;
        return NULL;
    }

    if( ( client = malloc( sizeof( echo_client_context_t ) ) ) == NULL )
    {
        echo_client_context_errno = ENOMEM;
        return NULL;
    }

    strcpy( client->eec_uname, uname );
    client->eec_tcp = ctx;

    return client;
}

void echo_client_context_destroy( echo_client_context_t *eec )
{
    tcp_context_destroy( eec->eec_tcp );
    free( eec );
}
