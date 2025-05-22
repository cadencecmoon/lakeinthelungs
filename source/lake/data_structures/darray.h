#pragma once

/** @file lake/data_structures/darray.h 
 *  @brief A set of macros for managing dynamically-allocated arrays.
 *  
 *  It removes the tedium of managing realloc'd arrays with pointer, size, 
 *  and allocated size, with support for the custom allocators.
 */
#include "../types.h"

/** A dynamic array expected to change in size. */
typedef struct {
    u8 *v;
    s32 size;
    s32 alloc;
} lake_darray;

/** Define a custom darray type. */
#define lake_darray_t(T) \
    union { lake_darray da; T *v; }
