#pragma once

#include "db3d/backend/backend.h"

EXPORT void d3_begin_default_pass(const d3_pass_action *pass_action, int width, int height);
EXPORT void d3_end_pass(void);