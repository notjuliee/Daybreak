//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_DB3D_GFX_H
#define DAYBREAK3D_GFX_DB3D_GFX_H

#include <daybreak/gfx/debug.h>
#include <daybreak/gfx/types.h>
#include <daybreak/gfx/version.h>
#include <daybreak/hedley.h>

#ifdef DB3D_GFX_COMPILATION
#define DB3D_GFX_API HEDLEY_PUBLIC
#else
#define DB3D_GFX_API HEDLEY_IMPORT
#endif

HEDLEY_BEGIN_C_DECLS

/**
 * Initialize the current graphics backend
 * @param desc A pointer to a filled in d3_desc struct, with the appropriate backend info filled.
 */
HEDLEY_NON_NULL(1)
DB3D_GFX_API void d3_setup(const d3_desc *desc);

/**
 * Shuts down the current graphics backend
 * Remember to destroy all objects created using the current backend before calling this method
 */
DB3D_GFX_API void d3_shutdown(void);

HEDLEY_WARN_UNUSED_RESULT
DB3D_GFX_API d3_backend d3_query_backend(void);

/**
 * Resets all cached state
 * If you make any API calls directly to the underlying graphics API, you _must_ call this method
 *   before calling any db3d_gfx methods
 */
DB3D_GFX_API void d3_reset_state_cache(void);

/**
 * Presents the current buffer to the window
 * This function may or may not block, depending on the implementation
 */
DB3D_GFX_API void d3_present(void);

/** @addtogroup debug Debugging Utilities
 * Various helpers to query the internal state of the current backend
 * @{
 */

/** Install a new set of trace hooks
 *
 * @param trace_hooks The new hooks to install
 * @return The previous hooks that were installed, these _should_ be called from your hooks
 */
HEDLEY_NON_NULL(1)
HEDLEY_WARN_UNUSED_RESULT
DB3D_GFX_API d3_dbg_trace_hooks d3_dbg_install_trace_hooks(HEDLEY_NO_ESCAPE const d3_dbg_trace_hooks *trace_hooks);

/** Push a new debug group
 *
 * Note: This only calls the push_debug_group trace hook
 * @param name The name of the debug group to push
 */
DB3D_GFX_API void d3_push_debug_group(const char *name);

/** Pop the last debug group
 *
 * Note: This only calls the pop_debug_group trace hook
 * @sa
 */
DB3D_GFX_API void d3_pop_debug_group(void);

/** Queries the internal state of a buffer
 * Note: This state is _internal_, and subject to change even in patch revisions
 */
HEDLEY_WARN_UNUSED_RESULT
HEDLEY_PURE
DB3D_GFX_API d3_dbg_buffer_info d3_dbg_query_buffer_info(d3_buffer buffer);

/** Queries the internal state of an image
 * Note: This state is _internal_, and subject to change even in patch revisions
 */
HEDLEY_WARN_UNUSED_RESULT
HEDLEY_PURE
DB3D_GFX_API d3_dbg_image_info d3_dbg_query_image_info(d3_image image);

/** Queries the internal state of a shader
 * Note: This state is _internal_, and subject to change even in patch revisions
 */
HEDLEY_WARN_UNUSED_RESULT
HEDLEY_PURE
DB3D_GFX_API d3_dbg_shader_info d3_dbg_query_shader_info(d3_shader shader);

/** @} */

/** @addtogroup pipeline Graphics pipeline
 * API to interact with the core graphics pipeline
 * @{
 */

HEDLEY_WARN_UNUSED_RESULT
HEDLEY_NON_NULL(1)
DB3D_GFX_API d3_pipeline d3_make_pipeline(const d3_pipeline_desc *desc);

DB3D_GFX_API void d3_destroy_pipeline(d3_pipeline pipeline);

DB3D_GFX_API void d3_apply_pipeline(d3_pipeline pipeline);

/** @} */

/** @addtogroup shader Graphics shaders
 * API to interact with graphics shaders
 */

HEDLEY_WARN_UNUSED_RESULT
HEDLEY_NON_NULL(1)
DB3D_GFX_API d3_shader d3_make_shader(const d3_shader_desc *desc);

DB3D_GFX_API void d3_destroy_shader(d3_shader shader);

/** @} */

HEDLEY_NON_NULL(1)
HEDLEY_NON_NULL(1)
DB3D_GFX_API void d3_begin_default_pass(const d3_pass_action *pass_action, int width, int height);

DB3D_GFX_API void d3_end_pass(void);
DB3D_GFX_API void d3_commit(void);

DB3D_GFX_API void d3_draw(int base_vertex, int num_vertices, int num_objects);

HEDLEY_END_C_DECLS

#endif // DAYBREAK3D_GFX_DB3D_GFX_H
