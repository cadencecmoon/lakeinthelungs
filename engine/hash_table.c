#include <amwe/hash_table.h>

lake_hot lake_pure 
u32 meiyan(const void *key, usize length)
{
    const u8 *raw = key;
    
    u32 h = 0x811c9dc5;
    while (length >= 8) {
        h = (h ^ ((((*(u32 *)raw) << 5) | ((*(u32 *)raw) >> 27)) ^ *(u32 *)(raw + 4))) * 0xad3e7;
        length -= 8;
        raw += 8;
    } 
#define ITER h = (h ^ *(u16 *)raw) * 0xad3e7; raw += 2;
    if (length & 4) { ITER ITER }
    if (length & 2) { ITER }
    if (length & 1) { h = (h ^ *raw) * 0xad3e7; }
#undef ITER
    return h ^ (h >> 16);
}

static struct hash_node *hash_node_new(
    const void   *key,
    usize         length,
    s32           value,
    struct riven *riven,
    riven_tag_t   tag)
{
    u8 *raw = (u8 *)riven_thalloc(riven, tag, lake_sizeof(struct hash_node) + length, lake_alignof(struct hash_node *));

    struct hash_node *node = (struct hash_node *)raw;
    node->length = length;
    node->key = (void *)&raw[lake_sizeof(struct hash_node)];
    bedrock_memcpy(node->key, key, length);
    node->next = NULL;
    node->value = value;
    return node;
}

void hash_table_init(
    struct hash_table *ht,
    struct riven      *riven,
    riven_tag_t        tag,
    u32                log2_size)
{
    bedrock_assert_debug(log2_size < 64);

    usize size = (1lu << log2_size);
    ht->size = size;

    lake_atomic_store_explicit(&ht->count, 0, lake_memory_model_relaxed);
    ht->table = (struct hash_node **)riven_thalloc(riven, tag, lake_sizeof(struct hash_node *) * size, lake_alignof(struct hash_node **));
    bedrock_memset(ht->table, 0, lake_sizeof(struct hash_node *) * size);
    ht->riven = riven;
    ht->tag = tag;
}

static void reinsert_when_resizing(
    struct hash_table *ht,
    struct hash_node  *node)
{
    u32 n = meiyan(&node->key, node->length) & (ht->size - 1);
    if (ht->table[n] == 0) {
        ht->table[n] = node;
        return;
    }
    struct hash_node *next = ht->table[n];
    node->next = next;
    ht->table[n] = node;
}

static void grow_table(struct hash_table *ht)
{
    usize o = ht->size;
    ht->size = (ht->size << 1);

    struct hash_node **old = ht->table;
    ht->table = (struct hash_node **)riven_thalloc(ht->riven, ht->tag, lake_sizeof(struct hash_node *) * ht->size, lake_alignof(struct hash_node **));
    for (usize i = 0; i < o; i++) {
        struct hash_node *node = old[i];
        while (node) {
            struct hash_node *next = node->next;
            node->next = NULL;
            reinsert_when_resizing(ht, node);
            node = next;
        }
    }
}

#include <string.h> /* memcmp */

bool hash_table_insert(
    struct hash_table *ht,
    const void        *key,
    usize              length,
    s32                value)
{
    u32 n = meiyan(key, length) & (ht->size - 1);
    if (ht->table[n] == 0) {
        if (lake_atomic_read_explicit(&ht->count, lake_memory_model_relaxed) == ht->size) {
            grow_table(ht);
            return hash_table_insert(ht, key, length, value);
        }
        lake_atomic_add_explicit(&ht->count, 1, lake_memory_model_release);
        ht->table[n] = hash_node_new(key, length, value, ht->riven, ht->tag);
        return false;
    }
    struct hash_node *node = ht->table[n];
    while (node) {
        if (node->length == length && memcmp(node->key, key, length) == 0) {
            node->value = value;
            return true;
        }
        node = node->next;
    }
    lake_atomic_add_explicit(&ht->count, 1, lake_memory_model_release);
    struct hash_node *new = hash_node_new(key, length, value, ht->riven, ht->tag);
    new->next = ht->table[n];
    ht->table[n] = new;
    return false;
}

bool hash_table_find(
    struct hash_table *ht,
    const void        *key,
    usize              length,
    s32              **out_value)
{
    u32 n = meiyan(key, length) & (ht->size - 1);
#if LAKE_HAS_BUILTIN(__builtin_prefetch)
    __builtin_prefetch(ht->table[n]);
#elif defined(LAKE_CC_MSVC_VERSION)
    _mm_prefetch((char *)ht->table[n], _MM_HINT_T0);
#endif
    struct hash_node *node = ht->table[n];
    if (!node) {
        *out_value = NULL;
        return false;
    }

    while (node) {
        if (node->length == length && memcmp(node->key, key, length) == 0) {
            *out_value = &node->value;
            return true;
        }
        node = node->next;
    }
    *out_value = NULL;
    return false;
}

s32 hash_table_for_each(
    struct hash_table *ht,
    PFN_hash_for_each  proc,
    void              *userdata)
{
    for (usize i = 0; i < ht->size; i++) {
        if (ht->table[i] != 0) {
            struct hash_node *node = ht->table[i];
            while (node) {
                s32 result = proc(node->key, node->length, &node->value, userdata);
                if (result != 0) 
                    return result;
                node = node->next;
            }
        }
    }
    return 0;
}
