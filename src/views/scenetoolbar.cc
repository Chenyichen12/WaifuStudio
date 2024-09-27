#include "scenetoolbar.h"
#include "iconbutton.h"
#include "flowlayout.h"

namespace views {
void SceneToolBar::handleControllerStateChanged(
    const WaifuL2d::SceneControllerState& state) {
  if (!state.isEdit) {
    this->setVisible(false);
    return;
  } else {
    this->setVisible(true);
  }

  auto toolIndex = static_cast<int>(state.editTool);
  for (int i = 0; i < 2; i++) {
    toolBtn[i]->setSelect(i == toolIndex);
  }
}

SceneToolBar::SceneToolBar(QWidget* parent) : QWidget(parent) {
  toolBtn[0] = new IconButton(this);
  toolBtn[0]->setIcon(":/icon/cursor-default.png");
  toolBtn[0]->setSelect(true);

  toolBtn[1] = new IconButton(this);
  toolBtn[1]->setIcon(":/icon/pen.png");
  toolBtn[1]->setSelect(false);

  for (auto btn : toolBtn) {
    btn->setFixedSize(20, 20);
    btn->setContentsMargins(0, 0, 0, 0);
    btn->setIconLength(15);
  }

  auto layout = new FlowLayout(this);
  layout->addWidget(toolBtn[0]);
  layout->addWidget(toolBtn[1]);

  setLayout(layout);
}

void SceneToolBar::setController(WaifuL2d::SceneController* controller) {
  connect(controller, &WaifuL2d::SceneController::stateChanged, this,
          &SceneToolBar::handleControllerStateChanged);
  connect(toolBtn[0], &IconButton::clicked, controller, [controller]() {
    controller->setEditTool(WaifuL2d::EditToolType::Cursor);
  });
  connect(toolBtn[1], &IconButton::clicked, controller, [controller]() {
    controller->setEditTool(WaifuL2d::EditToolType::AddPen);
  });
}
}