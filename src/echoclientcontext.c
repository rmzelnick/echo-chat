#include <string.h>
#include <errno.h>
#include "echoclientcontext.h"

void echo_client_context_strerror( int errnum, char *buf, size_t buflen )
{
    strerror_r( errnum, buf, buflen );
}

echo_client_context_t *echo_client_context_create( tcp_context_t *ctx,
        const char *uname, int *err )
{
    echo_client_context_t *client;

    if( ctx == NULL || uname == NULL )
    {
        *err = EINVAL;
        return NULL;
    }

    if( ( client = malloc( sizeof( echo_client_context_t ) ) ) == NULL )
    {
        *err = ENOMEM;
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
