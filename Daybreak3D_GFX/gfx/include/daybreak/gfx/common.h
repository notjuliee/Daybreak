//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_COMMON_H
#define DAYBREAK3D_GFX_COMMON_H

#if 0
#ifdef _WIN32
#ifdef D3_GFX_BUILD
#define D3_API __declspec(dllexport)
#else
#define D3_API __declspec(dllimport)
#endif
#endif
#else
#define D3_API
#endif

#endif // DAYBREAK3D_GFX_COMMON_H
