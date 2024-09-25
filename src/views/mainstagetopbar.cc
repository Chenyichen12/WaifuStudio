#include "mainstagetopbar.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPaintEvent>
#include "QPushButton"
#include <QPainter>

namespace views {
void TopBarIconBtn::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  if (this->selected) {
    painter.setBrush(Qt::gray);
  } else {
    painter.setBrush(Qt::white);
  }

  painter.drawRect(this->rect());
  auto center = rect().center();
  QRect imageRect(center.x() - 10, center.y() - 10, 20, 20);
  painter.drawImage(imageRect, icon);
}

void TopBarIconBtn::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);
}

void TopBarIconBtn::mouseReleaseEvent(QMouseEvent* event) {
  QWidget::mouseReleaseEvent(event);
  if (this->rect().contains(event->pos())) {
    emit clicked();
  }
}

TopBarIconBtn::TopBarIconBtn(QWidget* parent) : QWidget(parent) {
  this->selected = false;
  setMinimumWidth(30);
}

void TopBarIconBtn::setIcon(const QString& iconPath) {
  icon = QImage(iconPath);
  icon = icon.scaledToHeight(20, Qt::SmoothTransformation);
  update();
}

void TopBarIconBtn::setSelect(bool select) {
  this->selected = select;
  update();
}


MainStageTopBar::MainStageTopBar(QWidget* parent) : QWidget(parent) {
  auto layout = new QHBoxLayout(this);
  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

  this->editModeBtn = new TopBarIconBtn(this);
  editModeBtn->setIcon(":/icon/pen.png");
  layout->addWidget(editModeBtn);

  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
  this->setLayout(layout);
}


void MainStageTopBar::setController(
    const WaifuL2d::SceneController* sceneController) {
  connect(editModeBtn, &TopBarIconBtn::clicked, sceneController,
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