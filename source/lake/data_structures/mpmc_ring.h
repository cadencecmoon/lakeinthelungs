#pragma once

/** @file lake/data_structures/mpmc_ring.h 
 *  @brief Multiple-producer, multiple-consumer ring buffer.
 *
 *  The MPMC ring buffer was implemented from the multiple producer, multiple consumer (MPMC) 
 *  queue described by Dmitry Vyuko on 1024cores. [High Speed Atomic MPMC Queue]
 *
 *  http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 *
 *  Read and write operations on an atomic object are free from data races. However, if one thread 
 *  writes to it, all cache lines occupied by the object are invalidated. If another thread is reading
 *  from an unrelated object that shares the same cache line, it incures unnecesary overhead. This is 
 *  called false sharing, and we pad our MPMC ring buffer to avoid that. 
 *
 *  The ring buffer is limited to a cell count that is a power of two, as a mask is constructed:
 *  (cell_count - 1) == ((1u << log2_cell_count) - 1). The arrays for cells and data must be given via external means.
 */
#include "../types.h"
#include "../utility.h"
#include "../atomic.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A cell in the ring buffer. */
typedef struct {
    void        *data;
    atomic_usize sequence;
} lake_ring_cell;

/** Holds a ring buffer of cells and access points. */
typedef struct lake_alignment(LAKE_CACHELINE_SIZE) {
    lake_ring_cell *buffer;
    usize           buffer_mask;
    u8          pad0[LAKE_CACHELINE_SIZE - sizeof(lake_ring_cell *) - sizeof(usize)];

    atomic_usize    enqueue_pos;
    u8          pad1[LAKE_CACHELINE_SIZE - sizeof(atomic_usize)];

    atomic_usize    dequeue_pos;
    u8          pad2[LAKE_CACHELINE_SIZE - sizeof(atomic_usize)];
} lake_ring;

/** Typed wrapper around a MPMC ring cell. */
#define lake_ring_cell_t(T)  \
    union {                  \
        lake_ring_cell cell; \
        T             *v;    \
    }

/** Helper structure to enqueue/dequeue data. */
typedef struct {
    usize           position;
    lake_ring_cell *cell;
} lake_ring_entry;

/** Initializes a ring buffer with externally provided memory for cells and cell->data.
 *  Each `lake_ring_cell` in the buffer will point into an element of `array` at an offset 
 *  of `i * stride` for every `cell_count`. The cell count MUST BE a power of 2. 
 *
 *  The memory should have a lifetime no lesser than the expected use of the ring buffer.
 *  A data array is provided so that enqueue/dequeue can be submitted from transient data 
 *  (e.g. stack allocated), so consumers can operate directly on such data instead of 
 *  copying it back and forth. If the data should be externally managed, stride can be 
 *  set to 0 and the array as NULL. */
LAKE_NONNULL(1,2)
LAKEAPI void LAKECALL
lake_ring_init_(
    lake_ring          *ring,
    lake_ring_cell     *cells,
    usize               cell_count,
    usize               stride,
    void               *array);
#define lake_ring_init_t(ring, cells, log2_cell_count, T, array) \
    lake_ring_init_(ring, lake_reinterpret_cast(lake_ring_cell *, cells), (1lu << log2_cell_count), sizeof(T), lake_reinterpret_cast(void *, array))
#define lake_ring_init(ring, cells, log2_cell_count) \
    lake_ring_init_(ring, lake_reinterpret_cast(lake_ring_cell *, cells), (1lu << log2_cell_count), 0, nullptr)

/** Implements the means to consume from or submit data into the ring buffer. */
LAKE_NONNULL_ALL LAKE_HOT_FN
LAKEAPI void LAKECALL lake_ring_rotate(
    lake_ring          *ring,
    atomic_usize       *in_or_out,
    u32 const           pos_delta,
    lake_ring_entry    *output);

/** The producer. The data within cells is persistent, so submissions can be made from the stack.
 *  Everytime a enqueue happens to a cell, existing data is discarded (collisions won't happen). */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
bool lake_ring_enqueue_(
    lake_ring  *ring,
    usize       stride,
    void       *submit) 
{
    lake_ring_entry entry;
    lake_ring_rotate(ring, &ring->enqueue_pos, 0, &entry);
    
    if (entry.cell) {
        if (stride) {
            lake_dbg_assert(entry.cell->data, LAKE_INVALID_PARAMETERS, nullptr);
            lake_memcpy(entry.cell->data, submit, stride);
        } else {
            entry.cell->data = submit;
        }
        lake_atomic_write_explicit(&entry.cell->sequence, entry.position + 1, lake_memory_model_release);
        return true;
    }
    return false;
}
#define lake_ring_enqueue_t(ring, T, submit) \
    lake_ring_enqueue_(ring, sizeof(T), submit)
#define lake_ring_enqueue(ring, submit) \
    lake_rin_enqueue_(ring, 0, submit)

/** The consumer. It is enough to pass a pointer onto the output instead of copying the data. */
LAKE_FORCE_INLINE LAKE_NONNULL_ALL
bool lake_ring_dequeue_(
    lake_ring  *ring,
    usize       stride,
    void      **output)
{
    lake_ring_entry entry;
    lake_ring_rotate(ring, &ring->dequeue_pos, 1, &entry);

    if (entry.cell) {
        lake_dbg_assert(entry.cell->data, LAKE_INVALID_PARAMETERS, nullptr);
        *output = entry.cell->data;
        if (stride) entry.cell->data = nullptr;
        lake_atomic_write_explicit(&entry.cell->sequence, entry.position + ring->buffer_mask + 1, lake_memory_model_release);
        return true;
    }
    return false;
}

#define lake_ring_dequeue_t(ring, T, output) \
    lake_ring_dequeue_(ring, sizeof(T), output)
#define lake_ring_dequeue(ring, output) \
    lake_ring_dequeue_(ring, 0, output)

#ifdef __cplusplus
}
#endif /* __cplusplus */
