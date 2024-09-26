#include "scenetoolbar.h"
#include "iconbutton.h"
#include "flowlayout.h"

namespace views {
void SceneToolBar::handleControllerStateChanged(
    const WaifuL2d::SceneControllerState& state) {
  // if (state.isEdit == false) {
  // this->setVisible(false);
  // return;
  // }
}

SceneToolBar::SceneToolBar(QWidget* parent) : QWidget(parent) {
  toolBtn[0] = new IconButton(this);
  toolBtn[0]->setIcon(":/icon/cursor-default.png");
  toolBtn[0]->setSelect(true);

  toolBtn[1] = new IconButton(this);
  toolBtn[1]->setIcon(":/icon/pen.png");
  toolBtn[1]->setSelect(false);

  for (auto btn : toolBtn) {
    btn->setFixedSize(30, 30);
    btn->setContentsMargins(0, 0, 0, 0);
  }

  auto layout = new FlowLayout(this);
  layout->addWidget(toolBtn[0]);
  layout->addWidget(toolBtn[1]);

  setLayout(layout);
}

void SceneToolBar::setController(const WaifuL2d::SceneController* controller) {
  connect(controller, &WaifuL2d::SceneController::stateChanged, this,
          &SceneToolBar::handleControllerStateChanged);
}
}