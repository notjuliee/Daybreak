#ifndef DAYBREAK3D_EDITORWINDOW_H
#define DAYBREAK3D_EDITORWINDOW_H

#include <QMainWindow>
#include <DockManager.h>

namespace Ui {
class editorwindow;
}
class InfoWidget;
class EngineWidget;

class EditorWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow() noexcept;

    void LoadEngine();

  protected:
    void closeEvent(QCloseEvent *event) override;

  private:
    Ui::editorwindow *_ui;
    InfoWidget *_iw;
    EngineWidget *_ew;

    ads::CDockManager *_dm;
};

#endif // DAYBREAK3D_EDITORWINDOW_H
