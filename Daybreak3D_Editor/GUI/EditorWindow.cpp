#include "EditorWindow.h"

#include "Widgets/EngineWidget.h"
#include "Widgets/InfoWidget.h"
#include "ui_editorwindow.h"

#include <QCloseEvent>
#include <QSettings>
#include <DockAreaWidget.h>
#include <daybreak/gfx_loader/loader.h>

EditorWindow::EditorWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::editorwindow) {
    _ui->setupUi(this);
    _dm = new ads::CDockManager(this);
    _dm->setStyleSheet("");

    {
        QSettings settings;
        qDebug() << settings.fileName();
        QVariant state = settings.value("editor/state");
        if (!state.isNull()) {
            _dm->restoreState(state.value<QByteArray>());
        }
        _dm->loadPerspectives(settings);
    }

    _iw = new InfoWidget(this);
    auto *infoDock = new ads::CDockWidget("About");
    infoDock->setWidget(_iw);

    _ew = new EngineWidget(this);
    auto *engineDock = new ads::CDockWidget("Engine");
    engineDock->setWidget(_ew);
    auto *cdarea = _dm->setCentralWidget(engineDock);
    cdarea->setAllowedAreas(ads::AllDockAreas);

    _dm->addDockWidget(ads::LeftDockWidgetArea, infoDock);
    _dm->addDockWidget(ads::RightDockWidgetArea, engineDock);

    _dm->addToggleViewActionToMenu(infoDock->toggleViewAction());
    _dm->addToggleViewActionToMenu(engineDock->toggleViewAction());
    _ui->menuView->addMenu(_dm->viewMenu());
}

EditorWindow::~EditorWindow() noexcept {

    delete _iw;
    delete _ew;
    _dm->deleteLater();
    delete _ui;
}

void EditorWindow::closeEvent(QCloseEvent *event) {
    {
        QSettings settings;
        _dm->savePerspectives(settings);
        settings.setValue("editor/state", _dm->saveState());
    }
    event->accept();
}

void EditorWindow::LoadEngine() {
    _ew->InitEngine();
    _iw->initEngine();
}