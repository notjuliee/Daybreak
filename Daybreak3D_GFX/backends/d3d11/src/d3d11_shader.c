#include "d3d11_shader.h"

#include "db3d/backend/common_validation.h"
#include "db3d/backend/validation.h"
#include "state.h"

#include <stdio.h>

D3I_PRIVATE bool _load_compiler_dll(void) {
    if ((_sg.d3dcompiler_dll == 0) && !_sg.d3dcompiler_dll_load_failed) {
        _sg.d3dcompiler_dll = LoadLibraryA("d3dcompiler_47.dll");
        if (_sg.d3dcompiler_dll == 0) {
            printf("Failed to load d3dcompiler\n");
            _sg.d3dcompiler_dll_load_failed = true;
            return false;
        }
        _sg.D3DCompile_func = (pD3DCompile)GetProcAddress(_sg.d3dcompiler_dll, "D3DCompile");
        ASSERT(_sg.D3DCompile_func);
    }
    return _sg.d3dcompiler_dll != 0;
}

D3I_PRIVATE ID3DBlob *_compile_shader(const d3_shader_stage_desc *stage) {
    if (!_load_compiler_dll()) {
        return 0;
    }
    ASSERT(stage->d3d11_target);
    ID3DBlob *output = NULL;
    ID3DBlob *err = NULL;
    HRESULT hr = _sg.D3DCompile_func(stage->source, strlen(stage->source), NULL, NULL, NULL,
        stage->entry ? stage->entry : "main", stage->d3d11_target,
        D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &output, &err);
    if (err) {
        printf("%s\n", (const char *)err->lpVtbl->GetBufferPointer(err));
        err->lpVtbl->Release(err);
        err = 0;
    }
    if (FAILED(hr)) {
        if (output) {
            output->lpVtbl->Release(output);
            output = NULL;
        }
    }
    return output;
}

d3_resource_state d3i_create_shader(d_shader *shd, const d3_shader_desc *desc) {
    ASSERT(shd && desc);
    ASSERT(!shd->d3d11.vs && !shd->d3d11.fs && !shd->d3d11.vs_blob);
    HRESULT hr;
    ((void)sizeof(hr));

    d3i_shader_common_init(&shd->common, desc);

    for (int i = 0; i < D3_MAX_VERTEX_ATTRIBUTES; i++) {
        d3i_strcpy(&shd->d3d11.attrs[i].sem_name, desc->attrs[i].sem_name);
        shd->d3d11.attrs[i].sem_index = desc->attrs[i].sem_index;
    }

    for (int stage_index = 0; stage_index < D3_NUM_SHADER_STAGES; stage_index++) {
        d3i_shader_stage *common_stage = &shd->common.stages[stage_index];
        d_shader_stage *d_stage = &shd->d3d11.stage[stage_index];
        for (int ub_index = 0; ub_index < common_stage->num_uniform_blocks; ub_index++) {
            const d3i_uniform_block *ub = &common_stage->uniform_blocks[ub_index];

            ASSERT(d_stage->cbufs[ub_index] == 0);
            D3D11_BUFFER_DESC cb_desc = {
                .ByteWidth = d3i_roundup(ub->size, 16),
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            };
            hr = _sg.dev->lpVtbl->CreateBuffer(_sg.dev, &cb_desc, NULL, &d_stage->cbufs[ub_index]);
            ASSERT(SUCCEEDED(hr) && d_stage->cbufs[ub_index]);
        }
    }

    const void *vs_ptr = 0, *fs_ptr = 0;
    SIZE_T vs_length = 0, fs_length = 0;
    ID3DBlob *vs_blob = 0, *fs_blob = 0;
    if (desc->vs.byte_code && desc->fs.byte_code) {
        vs_ptr = desc->vs.byte_code;
        fs_ptr = desc->fs.byte_code;
        vs_length = desc->vs.byte_code_size;
        fs_length = desc->fs.byte_code_size;
    } else {
        vs_blob = _compile_shader(&desc->vs);
        fs_blob = _compile_shader(&desc->fs);
        if (vs_blob && fs_blob) {
            vs_ptr = vs_blob->lpVtbl->GetBufferPointer(vs_blob);
            vs_length = vs_blob->lpVtbl->GetBufferSize(vs_blob);
            fs_ptr = fs_blob->lpVtbl->GetBufferPointer(fs_blob);
            fs_length = fs_blob->lpVtbl->GetBufferSize(fs_blob);
        }
    }
    d3_resource_state res = D3_RESOURCESTATE_FAILED;

    if (vs_ptr && fs_ptr && (vs_length > 0) && (fs_length > 0)) {
        hr = _sg.dev->lpVtbl->CreateVertexShader(_sg.dev, vs_ptr, vs_length, NULL, &shd->d3d11.vs);
        bool vs_succ = SUCCEEDED(hr) && shd->d3d11.vs;
        hr = _sg.dev->lpVtbl->CreatePixelShader(_sg.dev, fs_ptr, fs_length, NULL, &shd->d3d11.fs);
        bool fs_succ = SUCCEEDED(hr) && shd->d3d11.vs;

        if (vs_succ && fs_succ) {
            shd->d3d11.vs_blob_length = vs_length;
            shd->d3d11.vs_blob = malloc(vs_length);
            ASSERT(shd->d3d11.vs_blob);
            memcpy(shd->d3d11.vs_blob, vs_ptr, vs_length);
            res = D3_RESOURCESTATE_VALID;
        }
    }
    if (vs_blob) {
        vs_blob->lpVtbl->Release(vs_blob);
        vs_blob = 0;
    }
    if (fs_blob) {
        fs_blob->lpVtbl->Release(fs_blob);
        fs_blob = 0;
    }

    return res;
}

d3_shader d3_make_shader(const d3_shader_desc *desc) {
    ASSERT_MSG(_sg.valid, "State must be valid");
    ASSERT_MSG(desc, "Desc must not be null");
    d3_shader_desc def = d3i_shader_desc_defaults(desc);
    d3_shader shd_id = d3i_alloc_d_shader();
    if (shd_id.id != D3_INVALID_ID) {
        d_shader *shd = d3i_lookup_d_shader(&_sg.pools.shaders, shd_id.id);
        ASSERT(shd && shd->slot.state == D3_RESOURCESTATE_ALLOC);
        shd->slot.ctx_id = _csg.active_context.id;
        if (d3i_validate_shader_desc(&def)) {
            shd->slot.state = d3i_create_shader(shd, &def);
        } else {
            shd->slot.state = D3_RESOURCESTATE_FAILED;
        }
        ASSERT((shd->slot.state == D3_RESOURCESTATE_VALID) || (shd->slot.state == D3_RESOURCESTATE_FAILED));
    } else {
        D3I_TRACE(err_shader_pool_exhausted);
    }
    D3I_TRACE_A(make_shader, &def, shd_id);
    return shd_id;
}

EXPORT void d3_destroy_shader(d3_shader shd_id) {
    ASSERT(_sg.valid);
    D3I_TRACE_A(destroy_shader, shd_id);

    d_shader *shd = d3i_lookup_d_shader(&_sg.pools.shaders, shd_id.id);
    if (!shd) {
        return;
    }
    if (shd->slot.ctx_id != _csg.active_context.id) {
        D3I_TRACE(err_context_mismatch);
        return;
    }
    if (shd->d3d11.vs) {
        shd->d3d11.vs->lpVtbl->Release(shd->d3d11.vs);
    }
    if (shd->d3d11.fs) {
        shd->d3d11.fs->lpVtbl->Release(shd->d3d11.fs);
    }
    if (shd->d3d11.vs_blob) {
        free(shd->d3d11.vs_blob);
    }
    for (int stage_index = 0; stage_index < D3_NUM_SHADER_STAGES; stage_index++) {
        d3i_shader_stage *cmn_stage = &shd->common.stages[stage_index];
        d_shader_stage *stage = &shd->d3d11.stage[stage_index];
        for (int ub_index = 0; ub_index < cmn_stage->num_uniform_blocks; ub_index++) {
            if (stage->cbufs[ub_index]) {
                stage->cbufs[ub_index]->lpVtbl->Release(stage->cbufs[ub_index]);
            }
        }
    }

    memset(shd, 0, sizeof(d_shader));
    d3i_pool_d_shader_free_index(&_sg.pools.shaders, d3i_slot_index(shd_id.id));
}
