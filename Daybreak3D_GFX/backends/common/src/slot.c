//
// Created by Julia on 1/15/2021.
//

#include "db3d/backend/slot.h"

#include <assert.h>
#include <string.h>

int d3i_slot_index(uint32_t id) {
    int slot_index = (int)(id & D3I_SLOT_MASK);
    assert(slot_index != D3I_INVALID_SLOT_INDEX);
    return slot_index;
}
void d3i_reset_slot(d3i_slot *slot) {
    assert(slot);
    memset(slot, 0, sizeof(d3i_slot));
}
uint32_t d3i_slot_alloc(d3i_ipool *pool, d3i_slot *slot, int slot_index) {
    assert(pool && pool->gen_ctrs);
    assert((slot_index > 0) && (slot_index < pool->size));
    assert((slot->state == D3_RESOURCESTATE_INITIAL) && (slot->id == D3_INVALID_ID));
    uint32_t ctr = ++pool->gen_ctrs[slot_index];
    slot->id = (ctr << D3I_SLOT_SHIFT) | (slot_index & D3I_SLOT_MASK);
    slot->state = D3_RESOURCESTATE_ALLOC;
    return slot->id;
}
