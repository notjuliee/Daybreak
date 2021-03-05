#ifndef DB_COMMON_POOLS_H
#define DB_COMMON_POOLS_H

#include <daybreak/hedley.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/** This shit is a lil janky, so read up
 * All items you create a pool for _must_ contain a field of type db_slot named slot
 */

HEDLEY_BEGIN_C_DECLS

enum {
    DB_INVALID_SLOT_INDEX = 0,
    DB_INVALID_ID = 0,
    DB_SLOT_SHIFT = 16,
    DB_SLOT_MASK = (1 << DB_SLOT_SHIFT) - 1,
};

typedef enum db_resource_state {
    DB_RESOURCESTATE_INITIAL = 0,
    DB_RESOURCESTATE_ALLOC,
    DB_RESOURCESTATE_VALID,
    DB_RESOURCESTATE_FAILED,
    DB_RESOURCESTATE_INVALID,
} db_resource_state;

typedef struct db_item {
    uint32_t id;
} db_item;

typedef struct db_slot {
    uint32_t id;
    uint32_t ctx_id;
    db_resource_state state;
} db_slot;

typedef struct db_pool {
    int size;
    size_t item_size;
    int queue_top;
    uint32_t *gen_ctrs;
    int *free_queue;
    void *items;
    uint32_t slot_offset;
} db_pool;

typedef bool (*db_pool_init_item)(void *item);

HEDLEY_WARN_UNUSED_RESULT
int db_slot_index(uint32_t id);

HEDLEY_WARN_UNUSED_RESULT
HEDLEY_NON_NULL(1, 2)
uint32_t db_slot_alloc(db_pool *pool, db_slot *slot, int slot_index) HEDLEY_REQUIRE(slot_index > 0);

#define db_init_pool(type, pool, num, callback)                                                                        \
    do {                                                                                                               \
        type t = {0};                                                                                                  \
        _db_init_pool(pool, sizeof(type), num, (void *)&t, &t.slot, callback);                                         \
    } while (0)

HEDLEY_NON_NULL(1, 4, 5)
void _db_init_pool(db_pool *pool, size_t size, int num, void *val, db_slot *val_s, db_pool_init_item callback)
    HEDLEY_REQUIRE(size > sizeof(db_slot)) HEDLEY_REQUIRE(num > 0);

HEDLEY_NON_NULL(1)
void db_free_pool(db_pool *pool);

HEDLEY_WARN_UNUSED_RESULT
HEDLEY_NON_NULL(1)
int db_pool_alloc_index(db_pool *pool);

HEDLEY_NON_NULL(1)
void db_pool_free_index(db_pool *pool, int slot_index) HEDLEY_REQUIRE(slot_index > 0);

HEDLEY_NON_NULL(1)
HEDLEY_RETURNS_NON_NULL
HEDLEY_WARN_UNUSED_RESULT
void *db_pool_at(const db_pool *pool, uint32_t id) HEDLEY_REQUIRE(id > 0);

HEDLEY_NON_NULL(1)
HEDLEY_WARN_UNUSED_RESULT
void *db_pool_lookup(const db_pool *pool, uint32_t id);

HEDLEY_NON_NULL(1)
HEDLEY_WARN_UNUSED_RESULT
db_item db_pool_alloc(db_pool *pool);

#define db_pool_alloc_t(t, pool) ((t){db_pool_alloc(pool).id})

HEDLEY_NON_NULL(1)
void db_pool_free(db_pool *pool, db_item item);

HEDLEY_END_C_DECLS

#endif
