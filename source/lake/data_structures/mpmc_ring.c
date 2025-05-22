#include "mpmc_ring.h"

void lake_ring_init_(
    lake_ring          *ring,
    lake_ring_cell     *cells,
    usize               cell_count,
    usize               stride,
    void               *array)
{
    lake_dbg_assert(lake_is_pow2(cell_count), LAKE_INVALID_PARAMETERS, "Ring buffer cell count must be a power of 2.");

    ring->buffer = cells;
    ring->buffer_mask = cell_count - 1; /* ((1lu << log2_cell_count) - 1) */

    if (array == nullptr) {
        for (usize i = 0lu; i < cell_count; i++)
            lake_atomic_write(&ring->buffer[i].sequence, i);
    } else {
        for (usize i = 0lu; i < cell_count; i++) {
            lake_atomic_write(&ring->buffer[i].sequence, i);
            ring->buffer[i].data = (void *)((u8 *)array + i * stride);
        }
    }
    lake_atomic_write(&ring->enqueue_pos, 0lu);
    lake_atomic_write(&ring->dequeue_pos, 0lu);
}

void lake_ring_rotate(
    lake_ring          *ring,
    atomic_usize       *in_or_out,
    u32 const           pos_delta,
    lake_ring_entry    *output)
{
    lake_ring_cell *cell;

    usize position = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);
    for (;;) {
        cell = &ring->buffer[position & ring->buffer_mask];

        usize sequence = lake_atomic_read_explicit(&cell->sequence, lake_memory_model_acquire);
        sptr difference = (sptr)sequence - (sptr)(position + pos_delta);

        if (!difference) {
            if (lake_atomic_compare_exchange_weak_explicit(in_or_out, &position, position + 1,
                    lake_memory_model_relaxed, lake_memory_model_relaxed))
            {
                *output = (lake_ring_entry){ .cell = cell, .position = position };
                return;
            }
        } else if (difference < 0) {
            /* it's empty */
            *output = (lake_ring_entry){ .cell = nullptr, .position = 0 };
            return;
        } else {
            position = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);
        }
    }
    LAKE_UNREACHABLE;
}
