#ifndef DAYBREAK3D_INFOWIDGET_H
#define DAYBREAK3D_INFOWIDGET_H

#include <QWidget>

namespace Ui {
class infowidget;
}

class InfoWidget : public QWidget {
    Q_OBJECT

  public:
    explicit InfoWidget(QWidget *parent = nullptr);
    ~InfoWidget() noexcept;

    void initEngine();

  private:
    Ui::infowidget *_ui;
};

#endif // DAYBREAK3D_INFOWIDGET_H
