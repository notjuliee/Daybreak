#include "EngineWidget.h"

#include "Ick/test_shaders.h"
#include <daybreak/gfx_loader/loader.h>

EngineWidget::EngineWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_DontCreateNativeAncestors, true);
    setAttribute(Qt::WA_NativeWindow, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
}
EngineWidget::~EngineWidget() noexcept {}

const char *vk_exts[2] = {"VK_KHR_win32_surface", "VK_KHR_surface"};

void EngineWidget::InitEngine() {
    d3_desc desc = {0};
    desc.context.sample_count = 1;
    desc.context.vulkan.extension_count = 2;
    desc.context.vulkan.extensions = vk_exts;
    desc.context.vulkan.win32.hwnd = (void *)winId();

    d3_setup(&desc);

    d3_shader_desc sd = {0};
    sd.vs.byte_code = vert_spv;
    sd.vs.byte_code_size = vert_spv_len;
    sd.fs.byte_code = frag_spv;
    sd.fs.byte_code_size = frag_spv_len;
    _sd = d3_make_shader(&sd);

    d3_pipeline_desc pd = {0};
    pd.shader = _sd;
    _pip = d3_make_pipeline(&pd);

    _pa = {0};
}

void EngineWidget::paintEvent(QPaintEvent *event) {
    d3_begin_default_pass(&_pa, width(), height());
    d3_apply_pipeline(_pip);
    d3_draw(0, 3, 1);
    d3_end_pass();
    d3_commit();
    d3_present();
}