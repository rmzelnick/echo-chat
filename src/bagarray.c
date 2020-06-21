#include "bagarray.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>

void bag_array_strerror( int errnum, char *buf, size_t buflen )
{
    if( errnum == ENOTFOUND )
    {
        strcpy( buf, "Element not found" );
    }
    else
    {
        strerror_r( errnum, buf, buflen );
    }
}

bag_array_t *bag_array_create( int *err )
{
    bag_array_t *bag;

    if( ( bag = malloc( sizeof( bag_array_t ) ) ) == NULL )
    {
        *err = ENOMEM;
        return NULL;
    }

    memset( bag, 0, sizeof( bag_array_t ) );

    return bag;
}

ssize_t bag_array_find_first( bag_array_t *bag, void *key, ssize_t *index,
       int ( *cmp )( const void*, const void* ), int *err )
{
    ssize_t i, retval;

    if( bag == NULL || key == NULL || index == NULL || cmp == NULL )
    {
        *err = EINVAL;
        return -1;
    }

    retval = -1;

    for( i = *index; i < bag->b_size; i++ )
    {
        if( cmp( key, bag->b_array[ i ] ) == 0 )
        {
            retval = i;
            break;
        }
    }

    if( i == bag->b_size )
        *err = ENOTFOUND;

    return retval;
}

void *bag_array_get( bag_array_t *bag, ssize_t index, int *err )
{
    if( bag == NULL || index >= bag->b_size )
    {
        *err = EINVAL;
        return NULL;
    }

    return bag->b_array[ index ];
}

int bag_array_insert( bag_array_t *bag, void *element, int *err )
{
    if( bag == NULL || element == NULL )
    {
        *err = EINVAL;
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
            *err = ENOMEM;
            return -1;
        }

        bag->b_array = tmp;
    }

    bag->b_array[ bag->b_size ] = element;
    bag->b_size++;

    return 0;
}

void *bag_array_remove( bag_array_t *bag, ssize_t index, int *err )
{
    void *tmp;

    if( bag == NULL || index >= bag->b_size )
    {
        *err = EINVAL;
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
