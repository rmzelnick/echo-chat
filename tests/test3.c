#include "bagarray.h"
#include <assert.h>

static int cmp( const void *a, const void *b );

int main( void )
{
    bag_array_t *bag;
    ssize_t from;
    int *n, i, key, err;

    assert( ( bag = bag_array_create( &err ) ) != NULL );

    for( i = 0; i < 1000; i++ )
    {
        n = malloc( sizeof( int ) );
        *n = 2 * i + 1;

        bag_array_insert( bag, n, &err );
    }

    assert( *( int* )bag_array_get( bag, 500, &err ) == 1001 );

    from = 0;
    key = 1001;
    assert( bag_array_find_first( bag, &key, &from, cmp, &err ) != -1 );

    for( i = 0; i < 1000; i++ )
    {
        free( bag_array_get( bag, i, &err ) );
    }

    bag_array_destroy( bag );

    return EXIT_SUCCESS;
}

int cmp( const void *a, const void *b )
{
    return *( int* )a - *( int* )b;
}
