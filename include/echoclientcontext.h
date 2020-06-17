#ifndef ECHOCLIENTCONTEXT_H
#define ECHOCLIENTCONTEXT_H

/*! \file echoclientcontext.h
 *  \brief Contains definitions for the Echo client context
 */

#include "tcpcontext.h"
#define MAX_LENGTH 64

extern int echo_client_context_errno; /*!< Contains error codes for client context operations */

/*! Echo client context */
typedef struct
{
    char eec_uname[ MAX_LENGTH ];   /*!< Client's username */
    tcp_context_t *eec_tcp;         /*!< Client's TCP context */
} echo_client_context_t;

/*! \fn void echo_client_context_perror( const char *s )
 *  \brief Prints an error associated with the echo client context.
 *  \param[in] s The function name associated with the error.
 */
extern void echo_client_context_perror( const char *s );

/*! \fn echo_client_context_t *echo_client_context_create( tcp_context_t *ctx, const char *uname )
 *  \brief Creates a client context.
 *  \param[in] ctx The tcp context used for client communications.
 *  \param[in] uname The username associated with the client context.
 *  \return On success a new echo client context is returned. Otherwise NULL
 *  is returned and errno is set appropriately.
 *  \exception ENOMEM No memory available.
 *  \exception EINVAL Invalid argument provided.
 */
extern echo_client_context_t *echo_client_context_create( 
        tcp_context_t *ctx, const char *uname );

/*! \fn void echo_client_context_destroy( echo_client_context_t *eec )
 *  \brief Destroys an echo client context.
 *  \param[in] eec The context to be destroyed.
 */
extern void echo_client_context_destroy( echo_client_context_t *eec );

#endif /* ECHOCLIENTCONTEXT_H */
