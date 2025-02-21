#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>

struct hash_node {
    struct hash_node   *next;
    void               *key;
    usize               length;
    s32                 value;
};

struct hash_table {
    struct hash_node  **table;
    usize               size;
    at_usize            count;
    struct rivens      *riven;
    rivens_tag_t        tag;
};

/** Initializes the hash table with given parameters. When the lifetime of the hash table ends,
 *  it's memory can be freed via Riven, as there are no other resources inside the table. The 
 *  size of the table must be a power of 2. If the table is overfilled while inserting a key,
 *  it will grow by doubling its capacity. */
AMWAPI attr_nonnull(1,2)
void hash_table_init(
    struct hash_table *ht,
    struct rivens     *riven,
    rivens_tag_t       tag,
    u32                log2_size);

/** Inserts a key into the table and associates it with an integer value. Returns true if the key 
 *  already exists in the table, and if so, the existing value is updated. */
AMWAPI attr_nonnull(1,2)
b32 hash_table_insert(
    struct hash_table *ht,
    const void        *key,
    usize              length,
    s32                value);

/** Searches for a key inside the table, returns true if found or false otherwise. Writes this 
 *  value to out_value, or writes 0 if the key does not exist in the table. */
AMWAPI attr_hot attr_nonnull(1,2,4)
b32 hash_table_find(
    struct hash_table *ht,
    const void        *key,
    usize              length,
    s32              **out_value);

/** Runs custom procedures for traversing through the entire hash table. */
typedef s32 (*PFN_hash_for_each)(const void *key, usize length, s32 *value, void *data);

/** Runs the custom procedure for every key in the hash table. */
AMWAPI void hash_table_for_each(
    struct hash_table *ht,
    PFN_hash_for_each  proc,
    void              *data);
