//
// Created by Julia on 1/15/2021.
//

#include <daybreak/gfx_loader/loader.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stdio.h>

#include "test_shaders.h"

static bool should_render = true;

static void fb_cb(GLFWwindow *win, int width, int height) {
    printf("%d %d\n", width, height);
    should_render = (width + height > 0);
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        return -1;
    d3_loader_load(argv[1]);

    glfwInit();
    d3_backend backend = d3_query_backend();
    const char *name = "Unknown";
    switch (backend) {
    case D3_BACKEND_D3D11: name = "Direct3D 11"; break;
    case D3_BACKEND_VULKAN: name = "Vulkan"; break;
    default: break;
    }
    printf("Using backend %s\n", name);

    char name_buf[512] = {0};
    snprintf(name_buf, 512, "Daybreak3D Test (%s)", name);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow *win = glfwCreateWindow(1280, 720, name_buf, 0, 0);
    glfwSetFramebufferSizeCallback(win, fb_cb);

    d3_desc desc = {
        .context =
            {
                .sample_count = 1,
            },
    };

    if (backend == D3_BACKEND_VULKAN) {
        desc.context.vulkan.extensions = glfwGetRequiredInstanceExtensions(&desc.context.vulkan.extension_count);
        desc.context.vulkan.win32.hwnd = glfwGetWin32Window(win);
    } else if (backend == D3_BACKEND_D3D11) {
        desc.context.d3d11.hwnd = glfwGetWin32Window(win);
        desc.context.d3d11.w = 1280;
        desc.context.d3d11.h = 720;
    }

    d3_setup(&desc);

    const float vertices[] = {
        0.0f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
    };

    /*
    d3_buffer buf = d3_make_buffer(&(d3_buffer_desc){.size = sizeof(vertices), .content = vertices});

    d3_bindings binds = {.vertex_buffers = {[0] = buf}};
     */

    /*
    d3_shader_desc sd = {.attrs =
                             {
                                 [0].sem_name = "POS",
                                 [1].sem_name = "COLOR",
                             },
        .vs.source = "struct vs_in {\n"
                     "  float3 pos: POS;\n"
                     "  float4 color: COLOR;\n"
                     "};\n"
                     "struct vs_out {\n"
                     "  float4 color: COLOR0;\n"
                     "  float4 pos: SV_Position;\n"
                     "};\n"
                     "vs_out main(vs_in inp) {\n"
                     "  vs_out outp;\n"
                     "  outp.pos = float4(inp.pos, 1.0);\n"
                     "  outp.color = inp.color;\n"
                     "  return outp;\n"
                     "}\n",
        .fs.source = "float4 main(float4 color: COLOR0): SV_Target0 {\n"
                     "  return color;\n"
                     "}\n"};
                     */
    d3_shader_desc sd = {
        .vs =
            {
                .byte_code = vert_spv,
                .byte_code_size = vert_spv_len,
            },
        .fs =
            {
                .byte_code = frag_spv,
                .byte_code_size = frag_spv_len,
            },
    };
    d3_shader shader = d3_make_shader(&sd);

    d3_pipeline pip = d3_make_pipeline(&(d3_pipeline_desc){
        .layout.attrs =
            {
                [0].format = D3_VERTEXFORMAT_FLOAT3,
                [1].format = D3_VERTEXFORMAT_FLOAT4,
            },
        .shader = shader,
    });
    d3_pass_action pass_action = {0};

    while (!glfwWindowShouldClose(win)) {
        if (should_render) {
            glfwPollEvents();
            d3_begin_default_pass(&pass_action, 1280, 720);
            d3_apply_pipeline(pip);
            // d3_apply_bindings(&binds);
            d3_draw(0, 3, 1);
            d3_end_pass();
            d3_commit();
            d3_present();
        } else {
            glfwWaitEvents();
        }
    }

    d3_destroy_pipeline(pip);
    d3_destroy_shader(shader);
    // d3_destroy_buffer(buf);

    d3_shutdown();
    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}