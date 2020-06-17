#include "bagarray.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>

int bag_errno;

static void signal_next( bag_array_t *bag );

void bag_array_perror( const char *s )
{
    if( bag_errno == ENOTFOUND )
    {
        printf( "%s: Element not found.\n", s );
    }
    else
    {
        errno = bag_errno;
        perror( s );
    }
}

bag_array_t *bag_array_create( void )
{
    bag_array_t *bag;

    if( ( bag = malloc( sizeof( bag_array_t ) ) ) == NULL )
    {
        bag_errno = ENOMEM;
        return NULL;
    }

    memset( bag, 0, sizeof( bag_array_t ) );

    return bag;
}

void *bag_array_get( bag_array_t *bag, size_t index )
{
    void *retval;

    pthread_mutex_lock( &bag->b_lock );

    while( bag->b_nwrites > 0 )
    {
        pthread_cond_wait( &bag->b_reader_cv, &bag->b_lock );
    }

    bag->b_nreads++;
    pthread_mutex_unlock( &bag->b_lock );

    if( bag == NULL || index >= bag->b_size )
    {
        bag_errno = EINVAL;
        return NULL;
    }

    retval = bag->b_array[ index ];

    pthread_mutex_lock( &bag->b_lock );
    bag->b_nreads--;
    signal_next( bag );
    pthread_mutex_unlock( &bag->b_lock );

    return retval;
}

ssize_t bag_array_find_first( bag_array_t *bag, void *key, ssize_t *index,
       int ( *cmp )( const void*, const void* ) )
{
    ssize_t i, retval;

    if( bag == NULL || key == NULL || index == NULL || cmp == NULL )
    {
        bag_errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock( &bag->b_lock );

    while( bag->b_nwrites > 0 )
    {
        pthread_cond_wait( &bag->b_reader_cv, &bag->b_lock );
    }

    bag->b_nreads++;
    pthread_mutex_unlock( &bag->b_lock );

    retval = -1;

    for( i = *index; i < ( ssize_t )bag->b_size; i++ )
    {
        if( cmp( key, bag->b_array[ i ] ) == 0 )
        {
            retval = i;
            break;
        }
    }

    pthread_mutex_lock( &bag->b_lock );
    bag->b_nreads--;
    signal_next( bag );
    pthread_mutex_unlock( &bag->b_lock );

    bag_errno = ENOTFOUND;
    return retval;
}

int bag_array_insert( bag_array_t *bag, void *element )
{
    if( bag == NULL || element == NULL )
    {
        bag_errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock( &bag->b_lock );
    bag->b_nwrites++;

    if( bag->b_nwrites > 1 || bag->b_nreads > 0 )
    {
        pthread_cond_wait( &bag->b_writer_cv, &bag->b_lock );
    }

    pthread_mutex_unlock( &bag->b_lock );

    if( bag->b_capacity == bag->b_size )
    {
        void **tmp;

        if( bag->b_capacity == 0 )
        {
            bag->b_capacity = 1;
        }
        else
        {
            bag->b_capacity *= 2;
        }

        tmp = realloc( bag->b_array, bag->b_capacity * sizeof( void* ) );

        if( tmp == NULL )
        {
            bag_errno = ENOMEM;
            pthread_mutex_lock( &bag->b_lock );
            bag->b_nwrites--;
            signal_next( bag );
            pthread_mutex_unlock( &bag->b_lock );

            return -1;
        }

        bag->b_array = tmp;
    }

    bag->b_array[ bag->b_size ] = element;
    bag->b_size++;

    pthread_mutex_lock( &bag->b_lock );
    bag->b_nwrites--;
    signal_next( bag );
    pthread_mutex_unlock( &bag->b_lock );

    return 0;
}

void *bag_array_remove( bag_array_t *bag, size_t index )
{
    void *tmp;

    pthread_mutex_lock( &bag->b_lock );
    bag->b_nwrites++;

    if( bag->b_nwrites > 1 || bag->b_nreads > 0 )
    {
        pthread_cond_wait( &bag->b_writer_cv, &bag->b_lock );
    }

    pthread_mutex_unlock( &bag->b_lock );

    if( bag == NULL || index >= bag->b_size )
    {
        bag_errno = EINVAL;
        pthread_mutex_lock( &bag->b_lock );
        bag->b_nwrites--;
        signal_next( bag );
        pthread_mutex_unlock( &bag->b_lock );

        return NULL;
    }

    tmp = bag->b_array[ index ];
    bag->b_array[ index ] = bag->b_array[ bag->b_size-1 ];
    bag->b_size--;

    pthread_mutex_lock( &bag->b_lock );
    bag->b_nwrites--;
    signal_next( bag );
    pthread_mutex_unlock( &bag->b_lock );

    return tmp;
}

void bag_array_destroy( bag_array_t *bag )
{
    if( bag->b_array != NULL )
    {
        free( bag->b_array );
    }

    free( bag );
}

void signal_next( bag_array_t *bag )
{
    if( bag->b_nwrites > 0 )
    {
        pthread_cond_signal( &bag->b_writer_cv );
    }
    else
    {
        pthread_cond_broadcast( &bag->b_reader_cv );
    }
}
