#ifndef TCPCONTEXT_H
#define TCPCONTEXT_H

/*! \file tcpcontext.h
 *  \brief Contains implementation of a TCP context.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

/*! TCP context for client-server communications */
typedef struct
{
    struct sockaddr_in tc_addr; /*!< TCP address */
    int tc_socket;              /*!< TCP socket  */
} tcp_context_t;

/*! \fn void tcp_context_strerror( int errnum, char *buf, size_t buflen )
 *  \brief Outputs message error associated with TCP context.
 *  \param[in] errnum The error code number.
 *  \param[out] buf The buffer that holds the error message.
 *  \param[in] buflen The length of the buffer.
 */
extern void tcp_context_strerror( int errnum, char *buf, size_t buflen );

/*! \fn tcp_context_t *tcp_context_create( int *err )
 *  \brief Creates a TCP connection context.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success a new context is returned. Otherwise NULL is returned
 *  and err parameter is set appropriately.
 *  \exception ENOMEM Not enough memory to allocate a context.
 *  \exception EACCESS Permission to create context is denied.
 *  \exception EMFILE The per-process limit on the number of open file
 *  descriptors has been reached.
 *  \exception ENFILE The system-wide limit on the total number of open
 *  files has been reached.
 */
extern tcp_context_t *tcp_context_create( int *err );

/*! \fn int *tcp_context_connect( tcp_context_t *ctx, const char *host, int port, int *err )
 *  \brief Connects a context to a hostname and port number.
 *  \param[in] ctx The context to be connected.
 *  \param[in] host The target hostname for connection.
 *  \param[in] port The target port number for connection.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success zero is returned. Otherwise -1 is returned and err
 *  parameter is set appropriately.
 *  \exception EADDRINUSE Local addresss already in use.
 *  \exception ECONNREFUSED No-one listening on the remote address.
 *  \exception EFAULT The socket structure address is outside the user's
 *  address space.
 *  \exception ENETUNREACH Network is unreachable.
 *  \exception ETIMEDOUT Timeout while attempting connection.
 */
extern int tcp_context_connect( tcp_context_t *ctx,
        const char *host, int port, int *err );

/*! \fn int tcp_context_bind( tcp_context_t *ctx, int port, int *err )
 *  \brief Binds a context to a port number.
 *  \param[in] ctx The context to be bound.
 *  \param[in] port The port number in which to bind the context.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success a socket is returned. Otherwise -1 is returned and
 *  err parameter is set appropriately.
 *  \exception EACCESS The address is protected, and the user is not
 *  superuser.
 *  \exception EADDRINUSE The given address is already in use.
 *  \exception EADDRINUSE The port number was specified asa zero in the
 *  socket address structure, but, upon attempting to bind to an ephemeral
 *  port, it was determined that all port numbers in the ephemeral range are
 *  currently in use.
 *  \exception EINVAL The socket is already bound to an address.
 */
extern int tcp_context_bind( tcp_context_t *ctx, int port, int *err );

/*! \fn int tcp_context_listen( tcp_context_t *ctx, int backlog, int *err )
 *  \brief Puts a context to listen for incoming connections.
 *  \param[in] ctx The context which listens.
 *  \param[in] backlog The maximum length to which the queue of pending
 *  connections may grow.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success zero is returned. Otherwise -1 is returned and err
 *  parameter is set appropriately.
 */
extern int tcp_context_listen( tcp_context_t *ctx, int backlog, int *err );

/*! \fn tcp_context_t *tcp_context_accept( const tcp_context_t *ctx, int *err )
 *  \brief Accepts an incoming connection.
 *  \param[in] ctx The server context.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success a new context of the incoming connection is returned.
 *  Otherwise NULL is returned and err parameter is set appropriately.
 *  \exception ECONNABORTED A connection has been aborted.
 *  \exception EINTR The system call was interrupted by a signal that was
 *  caught vefore a valid connection arrived.
 *  \exception EINVAL Context socket is not listening for connections.
 *  \exception EMFILE The per-process limit on the number of file
 *  descriptors has been reached.
 *  \exception ENFILE The system-wide limit on the total number of open
 *  files has been reached.
 *  \exception EPERM Firewall rules forbid connection.
 */
extern tcp_context_t *tcp_context_accept( const tcp_context_t *ctx,
        int *err );

/*! \fn ssize_t tcp_context_send( tcp_context_t *ctx, const char *buffer, size_t size, int *err )
 *  \brief Sends a message to another TCP context.
 *  \param[in] ctx The context to which the message is sent.
 *  \param[in] buffer The message buffer.
 *  \param[in] size The size (in bytes) of the buffer.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success the total number of bytes sent is returned. Otherwise
 *  -1 is returned and err parameter is set appropriately.
 *  \exception ECONNRESET Connection reset by peer.
 *  \exception EINTR A signal occurred before any data was transmitted.
 *  \exception ENOMEM No memory available.
 *  \exception ENOTCONN The socket is not connected, and no target has been
 *  give.
 *  \exception EPIPE The local context has been shutdown or destroyed.
 */
extern ssize_t tcp_context_send( tcp_context_t *ctx, const char *buffer,
        size_t size, int *err );

/*! \fn ssize_t tcp_context_recv( tcp_context_t *ctx, char *buffer, size_t size, int *err )
 *  \brief Receives a message from another TCP context.
 *  \param[in] ctx The context from which the message is received.
 *  \param[out] buffer The buffer containing the received message.
 *  \param[in] size Maximum size of buffer.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success the total number of bytes received is returned.
 *  Otherwise -1 is returned and err parameter is set appropriately.
 *  \exception ECONNRESET Connection reset by peer.
 *  \exception EINTR A signal occurred before any data was received.
 *  \exception ENOMEM No memory available.
 *  \exception ENOTCONN The socket is not connected, and no target has been
 *  give.
 *  \exception EPIPE The local context has been shutdown or destroyed.
 */
extern ssize_t tcp_context_recv( tcp_context_t *ctx, char *buffer,
        size_t size, int *err );

/*! \fn void tcp_context_destroy( tcp_context_t *ctx )
 *  \brief Destroys a TCP context.
 *  \param[in,out] ctx The context to be destroyed.
 */
extern void tcp_context_destroy( tcp_context_t *ctx );

#endif /* TCPCONTEXT_H */
