#pragma once
#include <QWidget>
#include "model/controller/scenecontroller.h"
class QPushButton;

namespace views {
class TopBarIconBtn : public QWidget {
  Q_OBJECT
  QImage icon;
  bool selected;

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

public:
  TopBarIconBtn(QWidget* parent = nullptr);
  void setIcon(const QString& iconPath);
  void setSelect(bool selected);
signals:
  void clicked();
};


class MainStageTopBar : public QWidget {
  Q_OBJECT

private:
  TopBarIconBtn* editModeBtn;
  // init when service init. it shouldn't be nullptr

public:
  MainStageTopBar(QWidget* parent = nullptr);
  /**
   * call it when service init set controller to scene controller
   * @param controller 
   */
  void setController(const WaifuL2d::SceneController* controller);

public slots:
  void handleControllerStateChanged(WaifuL2d::SceneControllerState state);
};
} // namespace views