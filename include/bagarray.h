#ifndef BAGARRAY_H
#define BAGARRAY_H

/*! \file bagarray.h
 *  \brief Contains implementation details of a bag array.
 */

#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#define ENOTFOUND 1000

/*! Implementation of Bag ADT using arrays. */
typedef struct
{
    ssize_t b_capacity; /*!< bag capacity */
    void **b_array;     /*!< pointer to array */
    ssize_t b_size;     /*!< bag size */
} bag_array_t;

/*! \fn void bag_array_strerror( int errnum, char *buf, size_t buflen )
 *  \brief Outputs a system error message.
 *  \param[in] errnum The error code number.
 *  \param[out] buf The buffer that holds the error message.
 *  \param[in] buflen The length of the buffer.
 */
extern void bag_array_strerror( int errnum, char *buf, size_t buflen );

/*! \fn bag_array_t *bag_array_create( int *err )
 *  \brief Creates a bag array.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success a new bag is returned. Otherwise NULL is returned and
 *  err parameter is set appropriately.
 *  \exception ENOMEM Not enough memory.
 */
extern bag_array_t *bag_array_create( int *err );

/*! \fn ssize_t bag_array_find_first( bag_array_t *bag, void *key, ssize_t *index, int ( *cmp )( const void*,const void* ), int *err )
 *  \brief Finds the first element in the bag.
 *  \param[in] bag The bag to be searched.
 *  \param[in] key The key element to be found in the bag.
 *  \param[in,out] index A pointer to the array index of the found element,
 *  \param[in] cmp A comparison function for elements.
 *  \param[out] err The error code returned in case of failure.
 *  and from where to start the search.
 *  \return On success zero is returned. Otherwise -1 is returned and err
 *  parameter is set appropriately.
 *  \exception EINVAL Invalid argument provided.
 *  \exception ENOTFOUND Element not found.
 */
extern ssize_t bag_array_find_first( bag_array_t *bag, void *key,
        ssize_t *index, int ( *cmp )( const void*, const void* ),
        int *err );

/*! \fn void *bag_array_get( bag_array_t *bag, ssize_t index, int *err )
 *  \brief Gets an element from a bag array.
 *  \param[in] bag The bag from which the element is retrieved.
 *  \param[in] index The array index of an element.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success a pointer to the element is returned. Otherwise NULL
 *  is returned and err parameter is set appropriately.
 *  \exception ENOTFOUND Element not found.
 *  \exception EINVAL Invalid argument.
 */
extern void *bag_array_get( bag_array_t *bag, ssize_t index, int *err );

/*! \fn int bag_array_insert( bag_array_t *bag, void *element, int *err )
 *  \brief Inserts an element into a bag array.
 *  \param[in] bag The bag to which an element is inserted.
 *  \param[in] element The element to be inserted into the bag.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success zero is returned. Otherwise -1 is returned and err
 *  parameter set appropriately.
 *  \exception EINVAL Invalid argument provided.
 *  \exception ENOMEM Not enough memory to resize bag.
 */
extern int bag_array_insert( bag_array_t *bag, void *element, int *err );

/*! \fn void *bag_array_remove( bag_array_t *bag, ssize_t index, int *err )
 *  \brief Removes an element from the bag.
 *  \param[in] bag The bag from which the element is removed.
 *  \param[in] index The array index of the element to be removed.
 *  \param[out] err The error code returned in case of failure.
 *  \return On success a pointer to the element is returned. Otherwise NULL
 *  is returned and err parameter is set appropriately.
 *  \exception EINVAL Invalid argument provided.
 */
extern void *bag_array_remove( bag_array_t *bag, ssize_t index, int *err );

/*! \fn void bag_array_destroy( bag_array_t *bag )
 *  \brief Destroys a bag.
 *  \param[in] bag The bag to be destroyed.
 */
extern void bag_array_destroy( bag_array_t *bag );

#endif /* BAGARRAY_H */
