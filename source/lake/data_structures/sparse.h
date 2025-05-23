#pragma once

/** @file lake/data_structures/sparse.h 
 *  @brief Sparse set data structure.
 */
#include "darray.h"

typedef struct {
    /** Dense array with indices to sparse array. The dense array stores both alive and dead
     *  sparse indices. The `count` member keeps track of which sparse indices are alive. */
    lake_darray_t(u64)  dense;
    lake_darray         pages;      /**< Chunks with sparse arrays and data. */
    s32                 elem_size;  /**< Size of an element in the sparse array. */
    s32                 count;      /**< Number of alive entries in the dense array. */
    u64                 max_idx;    /**< Local max index, if no global was set. */
    /* TODO allocators */
} lake_sparse;

