#include "scenetoolbar.h"

#include "flowlayout.h"
#include "iconbutton.h"


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
  for (int i = 0; i < toolBtn.size(); i++) {
    toolBtn[i]->setSelect(i == toolIndex);
  }
}

SceneToolBar::SceneToolBar(QWidget* parent) : QWidget(parent) {
  toolBtn[0] = new IconButton(this);
  toolBtn[0]->setIcon(":/icon/cursor-default.png");
  toolBtn[0]->setSelect(true);

  toolBtn[1] = new IconButton(this);
  toolBtn[1]->setIcon(":/icon/pen-add.png");
  toolBtn[1]->setSelect(false);

  toolBtn[2] = new IconButton(this);
  toolBtn[2]->setIcon(":/icon/pen-remove.png");
  toolBtn[2]->setSelect(false);

  for (auto btn : toolBtn) {
    btn->setFixedSize(20, 20);
    btn->setContentsMargins(0, 0, 0, 0);
    btn->setIconLength(15);
  }

  auto layout = new FlowLayout(this);
  layout->addWidget(toolBtn[0]);
  layout->addWidget(toolBtn[1]);
  layout->addWidget(toolBtn[2]);

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
  connect(toolBtn[2], &IconButton::clicked, controller, [controller]() {
    controller->setEditTool(WaifuL2d::EditToolType::RemovePen);
  });
}
}  // namespace views