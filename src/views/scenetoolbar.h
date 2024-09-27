#pragma once
#include <QWidget>
#include "model/controller/scenecontroller.h"

namespace views {
class IconButton;

class SceneToolBar : public QWidget {
  Q_OBJECT
  std::array<IconButton*, 2> toolBtn;

private slots:
  void handleControllerStateChanged(
      const WaifuL2d::SceneControllerState& state);

public:
  explicit SceneToolBar(QWidget* parent = nullptr);
  void setController(WaifuL2d::SceneController* controller);
};
}