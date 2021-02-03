#include <daybreak/utils/assert.h>
#include <daybreak/utils/pools.h>

#include <stdlib.h>
#include <string.h>

#define SAFE_FREE(x)                                                                                                   \
    do {                                                                                                               \
        ASSERT(x);                                                                                                     \
        free(x);                                                                                                       \
        x = NULL;                                                                                                      \
    } while (0)

#define GET_SLOT(x) (*(db_slot *)((uint8_t *)x + pool->slot_offset))
#define GET_ITEM(x) ((void *)(((uint8_t *)pool->items) + (pool->item_size * x)))

int db_slot_index(uint32_t id) {
    int slot_index = (int)(id & DB_SLOT_MASK);
    ASSERT(slot_index != DB_INVALID_SLOT_INDEX);
    return slot_index;
}

uint32_t db_slot_alloc(db_pool *pool, db_slot *slot, int slot_index) {
    ASSERT(pool && pool->gen_ctrs);
    ASSERT((slot_index > 0) && (slot_index < pool->size));
    ASSERT((slot->state == DB_RESOURCESTATE_INITIAL) && (slot->id == DB_INVALID_ID));
    uint32_t ctr = ++pool->gen_ctrs[slot_index];
    slot->id = (ctr << DB_SLOT_SHIFT) | (slot_index & DB_SLOT_MASK);
    slot->state = DB_RESOURCESTATE_ALLOC;
    return slot->id;
}

void _db_init_pool(db_pool *pool, size_t size, int num, void *val, db_slot *val_slot, db_pool_init_item init) {
    ASSERT(size > sizeof(db_slot));
    ASSERT(num > 0);
    ASSERT(val && val_slot);
    ASSERT((intptr_t)val_slot >= (intptr_t)val);

    pool->slot_offset = (uintptr_t)val_slot - (uintptr_t)val;
    ASSERT(pool->slot_offset >= 0 && pool->slot_offset < size - sizeof(db_slot));

    pool->size = num + 1;
    pool->item_size = size;
    pool->queue_top = 0;
    size_t gen_ctrs_bytes = sizeof(uint32_t) * pool->size;
    pool->gen_ctrs = malloc(gen_ctrs_bytes);
    ASSERT(pool->gen_ctrs);
    memset(pool->gen_ctrs, 0, gen_ctrs_bytes);
    pool->free_queue = malloc(sizeof(int) * pool->size);
    ASSERT(pool->free_queue);
    for (int i = pool->size - 1; i >= 1; i--) {
        pool->free_queue[pool->queue_top++] = i;
    }
    size_t items_bytes = size * num;
    pool->items = malloc(items_bytes);
    ASSERT(pool->items);
    memset((uint8_t *)pool->items, 0, items_bytes);
    if (init) {
        for (int i = 1; i < num; i++) {
            init(GET_ITEM(i));
        }
    }
}

void db_free_pool(db_pool *pool) {
    ASSERT(pool);
    SAFE_FREE(pool->free_queue);
    SAFE_FREE(pool->gen_ctrs);
    SAFE_FREE(pool->items);
}

int db_pool_alloc_index(db_pool *pool) {
    ASSERT(pool && pool->free_queue);
    if (pool->queue_top > 0) {
        int slot_index = pool->free_queue[--pool->queue_top];
        ASSERT((slot_index > 0) && (slot_index < pool->size));
        return slot_index;
    }
    return DB_INVALID_SLOT_INDEX;
}

void db_pool_free_index(db_pool *pool, int slot_index) HEDLEY_REQUIRE(slot_index > 0) {
    ASSERT((slot_index > 0) && (slot_index < pool->size));
    ASSERT(pool && pool->free_queue);
    ASSERT(pool->queue_top < pool->size);
    pool->free_queue[pool->queue_top++] = slot_index;
    ASSERT(pool->queue_top < pool->size);
}

void *db_pool_at(const db_pool *pool, uint32_t id) {
    ASSERT(pool && id != DB_INVALID_ID);
    int slot_index = db_slot_index(id);
    ASSERT(slot_index < pool->size);
    return GET_ITEM(slot_index);
}

void *db_pool_lookup(const db_pool *pool, uint32_t id) {
    if (id != DB_INVALID_ID) {
        void *v = db_pool_at(pool, id);
        if (GET_SLOT(v).id == id) {
            return v;
        }
    }
    return NULL;
}

db_item db_pool_alloc(db_pool *pool) {
    db_item res;
    int slot_index = db_pool_alloc_index(pool);
    if (slot_index != DB_INVALID_SLOT_INDEX) {
        res.id = db_slot_alloc(pool, &GET_SLOT(GET_ITEM(slot_index)), slot_index);
    } else {
        res.id = DB_INVALID_ID;
    }
    return res;
}

void db_pool_free(db_pool *pool, db_item item) {
    ASSERT(item.id != DB_INVALID_ID);
    int slot_index = db_slot_index(item.id);
    ASSERT(slot_index < pool->size);
    db_slot *slot = &GET_SLOT(GET_ITEM(slot_index));
    ASSERT(slot && slot->state != DB_RESOURCESTATE_INITIAL);
    memset(slot, 0, sizeof(db_slot));
    db_pool_free_index(pool, slot_index);
}
