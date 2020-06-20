#include "bagarray.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>

int bag_errno;

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
    if( bag == NULL || index >= bag->b_size )
    {
        bag_errno = EINVAL;
        return NULL;
    }

    return bag->b_array[ index ];
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

    bag_errno = ENOTFOUND;
    retval = -1;

    for( i = *index; i < ( ssize_t )bag->b_size; i++ )
    {
        if( cmp( key, bag->b_array[ i ] ) == 0 )
        {
            bag_errno = 0;
            retval = i;
            break;
        }
    }

    return retval;
}

int bag_array_insert( bag_array_t *bag, void *element )
{
    if( bag == NULL || element == NULL )
    {
        bag_errno = EINVAL;
        return -1;
    }

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
            return -1;
        }

        bag->b_array = tmp;
    }

    bag->b_array[ bag->b_size ] = element;
    bag->b_size++;

    return 0;
}

void *bag_array_remove( bag_array_t *bag, size_t index )
{
    void *tmp;

    if( bag == NULL || index >= bag->b_size )
    {
        bag_errno = EINVAL;
        return NULL;
    }

    tmp = bag->b_array[ index ];
    bag->b_array[ index ] = bag->b_array[ bag->b_size-1 ];
    bag->b_size--;

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
