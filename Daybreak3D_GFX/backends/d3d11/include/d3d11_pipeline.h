#pragma once

#include "d3d11_types.h"

EXPORT d3_pipeline d3_make_pipeline(const d3_pipeline_desc *desc);
EXPORT void d3_destroy_pipeline(d3_pipeline pip_id);
EXPORT void d3_apply_pipeline(d3_pipeline pip_id);