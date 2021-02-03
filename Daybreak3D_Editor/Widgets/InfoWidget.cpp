#include "InfoWidget.h"

#include "ui_infowidget.h"

#include <QLibraryInfo>
#include <daybreak/gfx_loader/loader.h>

InfoWidget::InfoWidget(QWidget *parent) : QWidget(parent), _ui(new Ui::infowidget) {
    _ui->setupUi(this);
    _ui->editorVersion->setText("v0.1.0");
}

InfoWidget::~InfoWidget() noexcept { delete _ui; }

void InfoWidget::initEngine() {
    _ui->engineVersion->setText(
        QString("v%1.%2.%3")
            .arg(QString::number(DB3D_GFX_VERSION_MAJOR), QString::number(DB3D_GFX_VERSION_MINOR),
                QString::number(DB3D_GFX_VERSION_PATCH)));
    switch (d3_query_backend()) {
    case D3_BACKEND_VULKAN: _ui->enginePlugin->setText("Vulkan"); break;
    case D3_BACKEND_D3D11: _ui->enginePlugin->setText("Direct3D 11"); break;
    default: _ui->enginePlugin->setText("Unknown???");
    }
}