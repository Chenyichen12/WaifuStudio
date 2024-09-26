#pragma once
#include <QWidget>
#include "model/controller/scenecontroller.h"
class QPushButton;

namespace views {
class IconButton;

class MainStageTopBar : public QWidget {
  Q_OBJECT

private:
  IconButton* editModeBtn;
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