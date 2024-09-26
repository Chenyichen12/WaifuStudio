#include "mainstagetopbar.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include "iconbutton.h"

namespace views {
MainStageTopBar::MainStageTopBar(QWidget* parent) : QWidget(parent) {
  auto layout = new QHBoxLayout(this);
  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

  this->editModeBtn = new IconButton(this);
  editModeBtn->setIcon(":/icon/pen.png");
  layout->addWidget(editModeBtn);

  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
  this->setLayout(layout);
}


void MainStageTopBar::setController(
    const WaifuL2d::SceneController* sceneController) {
  connect(editModeBtn, &IconButton::clicked, sceneController,
          &WaifuL2d::SceneController::toggleEditMode);
  connect(sceneController, &WaifuL2d::SceneController::stateChanged, this,
          &MainStageTopBar::handleControllerStateChanged);
  connect(sceneController, &WaifuL2d::SceneController::warning, this,
          [this](const QString& message) {
            QMessageBox::warning(this, "Warning", message);
          });
}

void MainStageTopBar::handleControllerStateChanged(
    WaifuL2d::SceneControllerState state) {
  this->editModeBtn->setSelect(state.isEdit);
}
} // namespace views