#include "mainstagesidetoolbar.h"

#include <QPushButton>
#include <QVBoxLayout>
namespace views {
void MainStageSideToolBar::handleBtnPressed(int index) {
  auto btn = toolList[index];
  if (index == this->currentBtnIndex) {
    btn->setChecked(true);
    return;
  }

  if (btn->isChecked()) {
    currentBtnIndex = index;
    for (const auto& tool : toolList) {
      tool->setChecked(false);
    }
    btn->setChecked(true);
    emit switchTool(index);
  }
}

MainStageSideToolBar::MainStageSideToolBar(QWidget* parent) : QWidget(parent) {
  auto layout = new QVBoxLayout(this);
  this->setLayout(layout);

  auto selectToolBtn = new QPushButton(this);
  selectToolBtn->setCheckable(true);
  selectToolBtn->setStyleSheet(
      "QPushButton {"
      "  background-image: url(:/icon/move-arrow.png);"
      "  background-repeat: no-repeat;"
      "  background-position: center;"
      "  border: none;"
      "  outline: none;"
      "}"
      "QPushButton:checked {"
      "  background-color: gray;"
      "}");
  selectToolBtn->setFixedSize(30, 30);
  selectToolBtn->setChecked(true);
  currentBtnIndex = 0;
  this->toolList.append(selectToolBtn);

  auto rotateToolBtn = new QPushButton(this);
  rotateToolBtn->setCheckable(true);
  rotateToolBtn->setStyleSheet(
      "QPushButton {"
      "  background-image: url(:/icon/rotation.png);"
      "  background-repeat: no-repeat;"
      "  background-position: center;"
      "  border: none;"
      "  outline: none;"
      "}"
      "QPushButton:checked {"
      "  background-color: gray;"
      "}");
  rotateToolBtn->setFixedSize(30, 30);
  layout->addWidget(selectToolBtn);
  layout->addWidget(rotateToolBtn);
  toolList.append(rotateToolBtn);

  for (int i = 0; i < toolList.size(); ++i) {
    const auto& btn = toolList[i];
    connect(btn, &QPushButton::toggled, this,
            [this,i]() { this->handleBtnPressed(i); });
  }
}

void MainStageSideToolBar::setPositionFromRect(const QRect& rect) {
  this->move(rect.x(), rect.y() + 20);
  this->resize(50, rect.height() - 40);
}

int MainStageSideToolBar::getCurrentTool() const {
  return this->currentBtnIndex;
}
}  // namespace views
