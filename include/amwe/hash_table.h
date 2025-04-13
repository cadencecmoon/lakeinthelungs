#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct hash_node {
    struct hash_node   *next;
    void               *key;
    usize               length;
    s32                 value;
};

struct hash_table {
    struct hash_node  **table;
    usize               size;
    atomic_usize        count;
    struct riven       *riven;
    riven_tag_t         tag;
};

/** Initializes the hash table with given parameters. When the lifetime of the hash table ends,
 *  it's memory can be freed via Riven, as there are no other resources inside the table. The 
 *  size of the table must be a power of 2. If the table is overfilled while inserting a key,
 *  it will grow by doubling its capacity. */
LAKEAPI lake_nonnull(1,2) void LAKECALL 
hash_table_init(
    struct hash_table *ht,
    struct riven      *riven,
    riven_tag_t        tag,
    u32                log2_size);

/** Inserts a key into the table and associates it with an integer value. Returns true if the key 
 *  already exists in the table, and if so, the existing value is updated. */
LAKEAPI lake_nonnull(1,2) bool LAKECALL 
hash_table_insert(
    struct hash_table *ht,
    const void        *key,
    usize              length,
    s32                value);

/** Searches for a key inside the table, returns true if found or false otherwise. Writes this 
 *  value to out_value, or writes 0 if the key does not exist in the table. */
LAKEAPI lake_hot lake_nonnull(1,2,4) bool LAKECALL 
hash_table_find(
    struct hash_table *ht,
    const void        *key,
    usize              length,
    s32              **out_value);

/** Runs custom procedures for traversing through the entire hash table. */
typedef s32 (LAKECALL *PFN_hash_for_each)(const void *key, usize length, s32 *value, void *userdata);

/** Runs the custom procedure for every key in the hash table. */
LAKEAPI s32 LAKECALL
hash_table_for_each(
    struct hash_table *ht,
    PFN_hash_for_each  proc,
    void              *userdata);

#ifdef __cplusplus
}
#endif /* __cplusplus */
