#ifndef DAYBREAK3D_GFX_VERSION_H
#define DAYBREAK3D_GFX_VERSION_H

#include <daybreak/hedley.h>

#define DB3D_GFX_VERSION_MAJOR 0
#define DB3D_GFX_VERSION_MINOR 1
#define DB3D_GFX_VERSION_PATCH 0

#define DB3D_GFX_VERSION HEDLEY_ENCODE_VERSION(DB3D_VERSION_MAJOR, DB3D_VERSION_MINOR, DB3D_VERSION_PATCH)

#if !defined(DB3D_GFX_TARGET_VERSION)
#define DB3D_GFX_TARGET_VERSION DB3D_GFX_VERSION
#endif

#endif