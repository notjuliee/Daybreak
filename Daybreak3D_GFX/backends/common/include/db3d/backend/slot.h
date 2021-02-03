//
// Created by Julia on 1/15/2021.
//

#ifndef DAYBREAK3D_GFX_COMMON_SLOT_H
#define DAYBREAK3D_GFX_COMMON_SLOT_H

#include "types.h"

#include <daybreak/gfx/types.h>
#include <stdint.h>

int d3i_slot_index(uint32_t id);
void d3i_reset_slot(d3i_slot *slot);
uint32_t d3i_slot_alloc(d3i_ipool *pool, d3i_slot *slot, int slot_index);

#endif // DAYBREAK3D_GFX_SLOT_H
