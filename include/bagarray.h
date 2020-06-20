#ifndef BAGARRAY_H
#define BAGARRAY_H

/*! \file bagarray.h
 *  \brief Contains implementation details of a bag array.
 */

#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#define ENOTFOUND 1000

extern int bag_errno; /*!< Contains the error code for bag array operations */

/*! Implementation of Bag ADT using arrays. */
typedef struct
{
    size_t b_capacity;  /*!< bag capacity */
    void **b_array;     /*!< pointer to array */
    size_t b_size;      /*!< bag size */
} bag_array_t;

/*! \fn void bag_array_perror( const char *s )
 *  \brief Prints a system error message.
 *  \param[in] s The function name associated with the error.
 */
extern void bag_array_perror( const char *s );

/*! \fn bag_array_t *bag_array_create( void )
 *  \brief Creates a bag array.
 *  \return On success a new bag is returned. Otherwise NULL is returned and
 *  errno is set appropriately.
 *  \exception ENOMEM Not enough memory.
 */
extern bag_array_t *bag_array_create( void );

/*! \fn ssize_t bag_array_find_first( bag_array_t *bag, void *key, ssize_t *index, int ( *cmp )( const void*,const void* ) )
 *  \brief Finds the first element in the bag.
 *  \param[in] bag The bag to be searched.
 *  \param[in] key The key element to be found in the bag.
 *  \param[in,out] index A pointer to the array index of the found element,
 *  \param[in] cmp A comparison function for elements.
 *  and from where to start the search.
 *  \return On success zero is returned. Otherwise -1 is returned and errno
 *  is set appropriately.
 *  \exception EINVAL Invalid argument provided.
 *  \exception ENOTFOUND Element not found.
 */
extern ssize_t bag_array_find_first( bag_array_t *bag, void *key,
        ssize_t *index, int ( *cmp )( const void*, const void* ) );

/*! \fn void *bag_array_get( bag_array_t *bag, size_t index )
 *  \brief Gets an element from a bag array.
 *  \param[in] bag The bag from which the element is retrieved.
 *  \param[in] index The array index of an element.
 *  \return On success a pointer to the element is returned. Otherwise NULL
 *  is returned and errno is set appropriately.
 *  \exception ENOTFOUND Element not found.
 *  \exception EINVAL Invalid argument.
 */
extern void *bag_array_get( bag_array_t *bag, size_t index );

/*! \fn int bag_array_insert( bag_array_t *bag, void *element )
 *  \brief Inserts an element into a bag array.
 *  \param[in] bag The bag to which an element is inserted.
 *  \param[in] element The element to be inserted into the bag.
 *  \return On success zero is returned. Otherwise -1 is returned and errno
 *  set appropriately.
 *  \exception EINVAL Invalid argument provided.
 *  \exception ENOMEM Not enough memory to resize bag.
 */
extern int bag_array_insert( bag_array_t *bag, void *element );

/*! \fn void *bag_array_remove( bag_array_t *bag, size_t index )
 *  \brief Removes an element from the bag.
 *  \param[in] bag The bag from which the element is removed.
 *  \param[in] index The array index of the element to be removed.
 *  \return On success a pointer to the element is returned. Otherwise NULL
 *  is returned and errno is set appropriately.
 *  \exception EINVAL Invalid argument provided.
 */
extern void *bag_array_remove( bag_array_t *bag, size_t index );

/*! \fn void bag_array_destroy( bag_array_t *bag )
 *  \brief Destroys a bag.
 *  \param[in] bag The bag to be destroyed.
 */
extern void bag_array_destroy( bag_array_t *bag );

#endif /* BAGARRAY_H */
