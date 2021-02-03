#include "GUI/EditorWindow.h"

#include <QApplication>
#include <daybreak/gfx_loader/loader.h>

int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName("NotJulie");
    QCoreApplication::setOrganizationDomain("notjulie.dev");
    QCoreApplication::setApplicationName("Daybreak3D Editor");
    QApplication app(argc, argv);
    EditorWindow win;

    d3_loader_load("../plugins/gfx/PluginVulkan.dll");
    win.LoadEngine();

    win.show();


    int ret = app.exec();

    d3_shutdown();
    return ret;
}