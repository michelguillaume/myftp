/*
** EPITECH PROJECT, 2024
** cvector
** File description:
** Header file for a C dynamic vector similar to std::vector in C++
*/

#ifndef CVECTOR_H
    #define CVECTOR_H

    #include <unistd.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdio.h>

    #define VECTOR_SUCCESS 0
    #define VECTOR_FAILURE 1

/**
 * @brief Structure for the vector header.
 * Similar to std::vector, it only stores size and capacity.
 * Data starts immediately after this structure.
 */
typedef struct {
    size_t size;         // Number of elements currently in the vector
    size_t capacity;     // Total capacity allocated
    size_t initial_capacity;
} vector_header_t;

    #if defined(__GNUC__) || defined(__clang__)
        #define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
    #else
        #define WARN_UNUSED_RESULT
    #endif

    // Access the vector header
    #define VECTOR_HEADER(v) ((vector_header_t *)(v) - 1)

    // Get the size of the vector
    #define VECTOR_SIZE(v) (VECTOR_HEADER(v)->size)

    // Get the capacity of the vector
    #define VECTOR_CAPACITY(v) (VECTOR_HEADER(v)->capacity)

    // Check if the vector is empty
    #define VECTOR_EMPTY(v) (VECTOR_SIZE(v) == 0)

    // Get a pointer to the underlying data
    #define VECTOR_DATA(v) (v)

    // Get the first element
    #define VECTOR_FRONT(v) (v)

    // Get the last element
    #define VECTOR_BACK(v) ((v) && VECTOR_SIZE(v) > 0 ? (v) + VECTOR_SIZE(v) - 1 : NULL)

    // Create a new vector of type T with initial capacity c
    #define VECTOR(T, c) ((T *)vector_init(sizeof(T), (c)))

    // Push back n elements (from pointer val) to the vector
    #define VECTOR_PUSH_BACK(v, val, n) vector_push_back_impl((void**)&(v), (const void*)(val), (n), sizeof(*(v)))

    // Pop back n elements
    #define VECTOR_POP_BACK(v, n) vector_pop_back_impl((void**)&(v), (n), sizeof(*(v)))

    // Clear the vector (size = 0, but doesn't change capacity)
    #define VECTOR_CLEAR(v) (VECTOR_HEADER(v)->size = 0)

    // Destroy the vector completely (free memory)
    #define VECTOR_DESTROY(v) (free(VECTOR_HEADER(v)), (v) = NULL)

    // Reserve capacity
    #define VECTOR_RESERVE(v, n) vector_reserve_impl((void **)&(v), (n), sizeof(*(v)))

    // Resize the vector (increase or decrease size)
    #define VECTOR_RESIZE(v, new_size) vector_resize_impl((void **)&(v), (new_size), sizeof(*(v)))

    // Shrink to fit (reduce capacity to current size)
    #define VECTOR_SHRINK_TO_FIT(v) vector_shrink_to_fit_impl((void**)&(v), sizeof(*(v)))

// Insert elements at the specified index
#define vector_insert(v, index, val, count) vector_insert_impl((void**)&(v), (index), (val), (count), sizeof(*(v)))

// Erase elements at the specified index
#define vector_erase(v, index, count) vector_erase_impl((void**)&(v), (index), (count), sizeof(*(v)))


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize a new vector with given element size and capacity.
 *
 * @param item_size Size of each element.
 * @param capacity Initial capacity.
 * @return Pointer to the newly allocated vector, or NULL on failure.
 */
void *vector_init(size_t item_size, size_t capacity) WARN_UNUSED_RESULT;

/**
 * @brief Ensure the vector has enough capacity for at least item_count new elements.
 *
 * @param v Pointer to the vector.
 * @param item_count Number of additional elements required.
 * @param item_size Size of each element.
 * @return A pointer to the (possibly reallocated) vector, or NULL on failure.
 */
void *vector_ensure_capacity(void *v, size_t item_count, size_t item_size) WARN_UNUSED_RESULT;

/**
 * @brief Push back n elements from val to the end of the vector.
 *
 * @param v Address of the vector pointer.
 * @param val Pointer to the data to insert.
 * @param n Number of elements to insert.
 * @param item_size Size of each element.
 * @return VECTOR_SUCCESS if successful, VECTOR_FAILURE otherwise.
 */
int vector_push_back_impl(void **v, const void *val, size_t n, size_t item_size) WARN_UNUSED_RESULT;

/**
 * @brief Pop back n elements from the vector.
 *
 * @param v Address of the vector pointer.
 * @param n Number of elements to remove from the end.
 * @param item_size Size of each element.
 */
void vector_pop_back_impl(void **v, size_t n, size_t item_size);

/**
 * @brief Reserve capacity for at least 'c' elements.
 *
 * @param v Address of the vector pointer.
 * @param n Number of elements to reserve.
 * @param item_size Size of each element.
 * @return VECTOR_SUCCESS on success, VECTOR_FAILURE on failure.
 */
int vector_reserve_impl(void **v, size_t n, size_t item_size);

/**
 * @brief Resize the vector to have new_size elements.
 * If new_size > current_size, new elements are zero-initialized.
 * If new_size < current_size, elements are truncated.
 *
 * @param v Address of the vector pointer.
 * @param new_size New desired size.
 * @param item_size Size of each element.
 * @return VECTOR_SUCCESS on success, VECTOR_FAILURE on failure.
 */
int vector_resize_impl(void **v, size_t new_size, size_t item_size);

/**
 * @brief Shrink the capacity of the vector to match its current size.
 *
 * @param v Address of the vector pointer.
 * @param item_size Size of each element.
 * @return VECTOR_SUCCESS on success, VECTOR_FAILURE on failure.
 */
int vector_shrink_to_fit_impl(void **v, size_t item_size);

/**
 * @brief Insert an element at the specified index (like std::vector::insert).
 *
 * @param v Address of the vector pointer.
 * @param index Position where to insert the elements.
 * @param val Pointer to the element(s) to insert.
 * @param count Number of elements to insert.
 * @param item_size Size of each element.
 * @return VECTOR_SUCCESS on success, VECTOR_FAILURE on failure.
 */
int vector_insert_impl(void **v, size_t index, const void *val, size_t count, size_t item_size) WARN_UNUSED_RESULT;

/**
 * @brief Erase elements at the specified index (like std::vector::erase).
 *
 * @param v Address of the vector pointer.
 * @param index Position of the first element to erase.
 * @param count Number of elements to erase.
 * @param item_size Size of each element.
 */
void vector_erase_impl(void **v, size_t index, size_t count, size_t item_size);

#ifdef __cplusplus
}
#endif

#endif //CVECTOR_H
