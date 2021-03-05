//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_PRIV_UTILS_H
#define DAYBREAK3D_GFX_PRIV_UTILS_H

#define SAFE_RELEASE(obj)                                                                                              \
    do {                                                                                                               \
        if (obj) {                                                                                                     \
            obj->lpVtbl->Release(obj);                                                                                 \
        }                                                                                                              \
    } while (0)
#define D3DFN(obj, interface, ...) obj->lpVtbl->interface(obj, __VA_ARGS__)

#endif // DAYBREAK3D_GFX_PRIV_UTILS_H
