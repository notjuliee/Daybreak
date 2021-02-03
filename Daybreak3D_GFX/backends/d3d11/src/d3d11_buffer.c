#include "d3d11_buffer.h"

#include "d3d11_converters.h"
#include "d3d11_pools.h"
#include "db3d/backend/common_validation.h"
#include "db3d/backend/validation.h"
#include "state.h"

d3_resource_state d_create_buffer(d_buffer *buf, const d3_buffer_desc *desc) {
    ASSERT_MSG(buf, "Buffer must be valid");
    ASSERT_MSG(desc, "Description must be valid");
    ASSERT_MSG(!buf->d3d11.buf, "Must not already have a D3D11 buffer");
    d3i_buffer_common_init(&buf->common, desc);
    const bool injected = (desc->d3d11_buffer != 0);
    if (injected) {
        buf->d3d11.buf = (ID3D11Buffer *)desc->d3d11_buffer;
        buf->d3d11.buf->lpVtbl->AddRef(buf->d3d11.buf);
    } else {
        D3D11_BUFFER_DESC d3d11_desc = {
            .ByteWidth = buf->common.size,
            .Usage = d_usage(buf->common.usage),
            .BindFlags =
                buf->common.type == D3_BUFFERTYPE_VERTEXBUFFER ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = d_cpu_access_flags(buf->common.usage),
        };
        D3D11_SUBRESOURCE_DATA *init_data_ptr = 0;
        D3D11_SUBRESOURCE_DATA init_data = {0};
        if (buf->common.usage == D3_USAGE_IMMUTABLE) {
            ASSERT_MSG(desc->content, "Desc must have content");
            init_data.pSysMem = desc->content;
            init_data_ptr = &init_data;
        }
        HRESULT hr = _sg.dev->lpVtbl->CreateBuffer(_sg.dev, &d3d11_desc, init_data_ptr, &buf->d3d11.buf);
        ASSERT_MSG(SUCCEEDED(hr) && buf->d3d11.buf, "Buffer must have been created");
    }
    return D3_RESOURCESTATE_VALID;
}

d3_buffer d3_make_buffer(const d3_buffer_desc *desc) {
    ASSERT_MSG(_sg.valid, "State must be valid");
    ASSERT_MSG(desc, "Desc must not be null");
    d3_buffer_desc def = d3i_buffer_desc_defaults(desc);
    d3_buffer buf_id = d3i_alloc_d_buffer();
    if (buf_id.id != D3_INVALID_ID) {
        d_buffer *buf = d3i_lookup_d_buffer(&_sg.pools.buffers, buf_id.id);
        ASSERT_MSG(buf && buf->slot.state == D3_RESOURCESTATE_ALLOC, "Buffer must be in right state");
        buf->slot.ctx_id = _csg.active_context.id;
        if (d3i_validate_buffer_desc(&def)) {
            buf->slot.state = d_create_buffer(buf, &def);
        } else {
            buf->slot.state = D3_RESOURCESTATE_FAILED;
        }
    } else {
        D3I_TRACE(err_buffer_pool_exhausted);
    }
    D3I_TRACE_A(make_buffer, &def, buf_id);
    return buf_id;
}

EXPORT void d3_destroy_buffer(d3_buffer buf_id) {
    ASSERT_MSG(_sg.valid, "State must be valid");
    D3I_TRACE_A(destroy_buffer, buf_id);

    d_buffer *buf = d3i_lookup_d_buffer(&_sg.pools.buffers, buf_id.id);
    if (!buf) {
        return;
    }
    if (buf->slot.ctx_id != _csg.active_context.id) {
        D3I_TRACE(err_context_mismatch);
        return;
    }

    if (buf->d3d11.buf) {
        buf->d3d11.buf->lpVtbl->Release(buf->d3d11.buf);
    }

    memset(buf, 0, sizeof(d_buffer));
    d3i_pool_d_buffer_free_index(&_sg.pools.buffers, d3i_slot_index(buf_id.id));
}

EXPORT d3_dbg_buffer_info d3_dbg_query_buffer_info(struct d3_buffer buf_id) {
    ASSERT(_csg.valid);
    d3_dbg_buffer_info info = {0};
    const d_buffer *buf = d3i_lookup_d_buffer(&_sg.pools.buffers, buf_id.id);
    if (buf) {
        info.slot.state = buf->slot.state;
        info.slot.res_id = buf->slot.id;
        info.slot.ctx_id = buf->slot.ctx_id;
        info.update_frame_index = buf->common.update_frame_index;
        info.append_frame_index = buf->common.append_frame_index;
        info.append_pos = buf->common.append_pos;
        info.append_overflow = buf->common.append_overflow;
        info.num_slots = 1;
        info.active_slot = 0;
    }
    return info;
}
