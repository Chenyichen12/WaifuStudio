#include "mainstagetopbar.h"

#include <QHBoxLayout>

#include "QPushButton"

namespace views {
MainStageTopBar::MainStageTopBar(QWidget* parent) : QWidget(parent) {
  auto layout = new QHBoxLayout(this);
  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

  this->editModeBtn = new QPushButton(tr("edit mesh"), this);
  layout->addWidget(editModeBtn);

  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
  this->setLayout(layout);
}


void MainStageTopBar::setController(WaifuL2d::SceneController* controller) {
  this->controller = controller;
  connect(editModeBtn, &QPushButton::clicked, controller,
          &WaifuL2d::SceneController::toggleEditMode);
  connect(controller, &WaifuL2d::SceneController::stateChanged, this,
          &MainStageTopBar::handleControllerStateChanged);
}

void MainStageTopBar::handleControllerStateChanged(
    WaifuL2d::SceneControllerState state) {
  this->editModeBtn->setCheckable(true);
  this->editModeBtn->setChecked(state.isEdit);
  this->editModeBtn->setCheckable(false);
}
} // namespace views