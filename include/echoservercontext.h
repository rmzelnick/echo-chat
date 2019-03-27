#ifndef ECHOSERVERCONTEXT_H
#define ECHOSERVERCONTEXT_H

/*! \file echoservercontext.h
 *  \brief Contains defitions for echo server context
 */

#include "tcpcontext.h"
#include "bagarray.h"
#include "echoclientcontext.h"
#define EDUPLICATE 14321

extern int echo_server_context_errno;
int echo_server_context_errno; /*!< Contains error codes for server context operations */

/*! Echo server context */
typedef struct
{
    tcp_context_t *esc_tcp; /*!< Echo server's TCP context */
    bag_array_t *esc_bag;   /*!< Echo server's bag of clients */
} echo_server_context_t;

/*! \fn void echo_server_context_perror( const char *s )
 *  \brief Prints a server context error message.
 *  \param[in] s The function name associated with the error.
 */
extern void echo_server_context_perror( const char *s );

/*! \fn echo_server_context_t *echo_server_context_create( tcp_context_t *ctx )
 *  \brief Creates an echo server context.
 *  \param[in] ctx The TCP context used for server communication.
 *  \return On success a new echo server context is returned. Otherwise NULL
 *  is returned and errno is set appropriately.
 *  \exception ENOMEM No memory available.
 *  \exception EINVAL Invalid argument provided.
 */
extern echo_server_context_t *echo_server_context_create(
        tcp_context_t *ctx );

/*! \fn int echo_server_context_insert( echo_server_context_t *ctx, echo_client_context_t *client )
 *  \brief Inserts a client context into the server context's bag.
 *  \param[in] ctx The context to which insert the client.
 *  \param[in] client The client context to be inserted.
 *  \return On success zero is returned. Otherwise -1 is returned and errno
 *  is set appropriately.
 *  \exception EDUPLICATE Duplicate context insertion.
 *  \exception EINVAL Invalid argument provided.
 */
extern int echo_server_context_insert( echo_server_context_t *ctx,
        echo_client_context_t *client );

/*! \fn tcp_context_t *echo_server_context_remove( echo_server_context_t *ctx, echo_client_context *client )
 *  \brief Removes a client context from the server's bag.
 *  \param[in] ctx The server context from which to remove the client.
 *  \param[in] client The client to be removed from the bag.
 *  \return On success the client removed from the bag is returned.
 *  Otherwise NULL is returned and errno is set appropriately.
 *  \exception EINVAL Invalid argument provided.
 */
extern tcp_context_t *echo_server_context_remove( echo_server_context_t *ctx,
        echo_client_context_t *client );

/*! \fn void echo_server_context_sendall( echo_server_context_t *ctx, const char *buffer, size_t size )
 *  \brief Sends a message to all clients.
 *  \param[in] ctx The TCP context used for server communication.
 *  \param[in] buffer The message to be echoed to all clients.
 *  \param[in] size The message size in bytes.
 */
extern void echo_server_context_sendall( echo_server_context_t *ctx,
        const char *buffer, size_t size );

/*! \fn void echo_server_context_destroy( echo_server_context_t *ctx )
 *  \brief Destroys an echo server context.
 *  \param[in] ctx The server context to be destroyed.
 */
extern void echo_server_context_destroy( echo_server_context_t *ctx );

#endif /* ECHOSERVERCONTEXT_H */
