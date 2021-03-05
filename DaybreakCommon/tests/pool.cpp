#include <daybreak/utils/pools.h>

#include <catch2/catch.hpp>

struct test_item {
    db_slot slot;
    uint32_t uwu;
};

bool init_test_item(test_item *item) {
    item->uwu = 69;
    return true;
}

SCENARIO("Pools can be created") {
    WHEN("A pool is created") {
        db_pool test_pool;
        db_init_pool(test_item, &test_pool, 8, nullptr);

        THEN("the pool must exist") {
            REQUIRE(test_pool.size == 8 + 1);
            REQUIRE(test_pool.items != nullptr);
        }

        db_free_pool(&test_pool);
    }
}

SCENARIO("Pools can allocate and free indices") {
    GIVEN("A pool of 8 items") {
        db_pool test_pool;
        db_init_pool(test_item, &test_pool, 8, nullptr);

        WHEN("an index is allocated") {
            int indx = db_pool_alloc_index(&test_pool);

            THEN("an index is created") { REQUIRE(indx > DB_INVALID_SLOT_INDEX); }
            THEN("the queue top goes down") { REQUIRE(test_pool.queue_top == 7); }

            WHEN("that index is freed") {
                db_pool_free_index(&test_pool, indx);
                THEN("the queue top goes up") { REQUIRE(test_pool.queue_top == 8); }
            }
        }

        db_free_pool(&test_pool);
    }
}

SCENARIO("Pools can allocate and free items") {
    GIVEN("A pool of 8 items") {
        db_pool test_pool;
        db_init_pool(test_item, &test_pool, 8, reinterpret_cast<db_pool_init_item>(init_test_item));

        WHEN("an item is allocated") {
            db_item item = db_pool_alloc(&test_pool);
            test_item *ti = (test_item *)db_pool_lookup(&test_pool, item.id);

            THEN("we get an item") {
                REQUIRE(item.id > DB_INVALID_ID);
                REQUIRE(ti != NULL);
                REQUIRE(ti->uwu == 69);
            }

            WHEN("a second item is allocated") {
                db_item item2 = db_pool_alloc(&test_pool);
                test_item *ti2 = (test_item *)db_pool_lookup(&test_pool, item2.id);

                THEN("we get another item") {
                    REQUIRE(item2.id > DB_INVALID_ID);
                    REQUIRE(item2.id != item.id);
                    REQUIRE(ti2 != NULL);
                    REQUIRE(ti2->uwu == 69);
                }

                WHEN("the second item is freed") {
                    db_pool_free(&test_pool, item2);

                    THEN("the item is gone") {
                        REQUIRE(ti2->slot.state == DB_RESOURCESTATE_INITIAL);
                        REQUIRE(test_pool.queue_top == 7);
                    }
                }
            }

            WHEN("the first item is freed") {
                db_pool_free(&test_pool, item);

                THEN("the item is gone") {
                    REQUIRE(ti->slot.state == DB_RESOURCESTATE_INITIAL);
                    REQUIRE(test_pool.queue_top == 8);
                }
            }
        }

        db_free_pool(&test_pool);
    }
}