#include <amw/hash_table.h>
#include <amw/log.h>

attr_hot attr_pure u32 meiyan(const void *key, usize length)
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
    const void    *key,
    usize          length,
    s32            value,
    struct rivens *riven,
    rivens_tag_t   tag)
{
    u8 *raw = (u8 *)riven_alloc(riven, tag, sizeof(struct hash_node) + length, alignof(struct hash_node *));

    struct hash_node *node = (struct hash_node *)raw;
    node->length = length;
    node->key = (void *)&raw[sizeof(struct hash_node)];
    memcpy(node->key, key, length);
    node->next = NULL;
    node->value = value;
    return node;
}

void hash_table_init(
    struct hash_table  *ht,
    struct rivens      *riven,
    rivens_tag_t        tag,
    u32                 log2_size)
{
    assert_debug(log2_size < 64);

    usize size = (1lu << log2_size);
    ht->size = size;

    atomic_store_explicit(&ht->count, 0, memory_order_relaxed);
    ht->table = (struct hash_node **)riven_alloc(riven, tag, sizeof(struct hash_node *) * size, alignof(struct hash_node **));
    memset(ht->table, 0, sizeof(struct hash_node *) * size);
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
    ht->table = (struct hash_node **)riven_alloc(ht->riven, ht->tag, sizeof(struct hash_node *) * ht->size, alignof(struct hash_node **));
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

b32 hash_table_insert(
    struct hash_table  *ht,
    const void         *key,
    usize               length,
    s32                 value)
{
    u32 n = meiyan(key, length) & (ht->size - 1);
    if (ht->table[n] == 0) {
        if (atomic_load_explicit(&ht->count, memory_order_relaxed) == ht->size) {
            grow_table(ht);
            return hash_table_insert(ht, key, length, value);
        }
        atomic_fetch_add_explicit(&ht->count, 1, memory_order_release);
        ht->table[n] = hash_node_new(key, length, value, ht->riven, ht->tag);
        return false;
    }
    struct hash_node *node = ht->table[n];
    while (node) {
        if (node->length == length && memcmp(node->key, key, length) == 0)
            return true;
        node = node->next;
    }
    atomic_fetch_add_explicit(&ht->count, 1, memory_order_release);
    struct hash_node *new = hash_node_new(key, length, value, ht->riven, ht->tag);
    new->next = ht->table[n];
    ht->table[n] = new;
    return false;
}

b32 hash_table_find(
    struct hash_table  *ht,
    const void         *key,
    usize               length,
    s32               **out_value)
{
    u32 n = meiyan(key, length) & (ht->size - 1);
#if defined(CC_GNUC_VERSION)
    __builtin_prefetch(ht->table[n]);
#elif defined(CC_MSVC_VERSION)
    _mm_prefetch((char *)ht->table[n], _MM_HINT_T0);
#else
#error implement a prefetch for this target 
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

void hash_table_for_each(
    struct hash_table  *ht,
    PFN_hash_for_each   fn,
    void               *data)
{
    for (usize i = 0; i < ht->size; i++) {
        if (ht->table[i] != 0) {
            struct hash_node *node = ht->table[i];
            while (node) {
                if (fn(node->key, node->length, &node->value, data) != result_reiterate) return;
                node = node->next;
            }
        }
    }
}
