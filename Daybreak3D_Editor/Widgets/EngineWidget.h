#ifndef DAYBREAK3D_ENGINEWIDGET_H
#define DAYBREAK3D_ENGINEWIDGET_H

#include <QWidget>
#include <daybreak/gfx/types.h>

class EngineWidget : public QWidget {
    Q_OBJECT

  public:
    explicit EngineWidget(QWidget *parent = nullptr);
    ~EngineWidget() noexcept;

    void InitEngine();

  protected:
    QPaintEngine *paintEngine() const override { return nullptr; }
    void paintEvent(QPaintEvent *event) override;

  private:
    d3_shader _sd;
    d3_pipeline _pip;
    d3_pass_action _pa;
};

#endif // DAYBREAK3D_ENGINEWIDGET_H
