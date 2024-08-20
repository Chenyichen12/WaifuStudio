#include "mainstagesidetoolbar.h"

#include <QPushButton>
#include <QVBoxLayout>
namespace views {
void MainStageSideToolBar::handleSwitchTool(int index) {
  currentBtnIndex = index;
  for (int i = 0; i < toolList.size(); i++) {
    if (i == index) {
      toolList[i]->setChecked(true);
    } else {
      toolList[i]->setChecked(false);
    }
  }
  emit switchTool(index);
}

QString MainStageSideToolBar::generateButtonStyleSheet(
    const QString& iconPath) {
  return QString(
             "QPushButton {"
             "  background-image: url(%1);"
             "  background-repeat: no-repeat;"
             "  background-position: center;"
             "  border: none;"
             "  outline: none;"
             "}"
             "QPushButton:checked {"
             "  background-color: gray;"
             "}")
      .arg(iconPath);
}

QPushButton* MainStageSideToolBar::addBtn(const QString& iconPath) {
  auto btn = new QPushButton(this);
  btn->setCheckable(true);
  btn->setStyleSheet(generateButtonStyleSheet(iconPath));
  btn->setFixedSize(30, 30);
  btn->setChecked(false);
  this->toolList.append(btn);
  return btn;
}

MainStageSideToolBar::MainStageSideToolBar(QWidget* parent) : QWidget(parent) {
  auto layout = new QVBoxLayout(this);
  this->setLayout(layout);

  auto arrow = addBtn(":/icon/move-arrow.png");
  arrow->setChecked(true);
  currentBtnIndex = 0;

  addBtn(":/icon/rotation.png");
  auto penBtn = addBtn(":/icon/pen.png");
  this->setEnableTool(2, false);

  for (int i = 0; i < toolList.size(); ++i) {
    const auto& btn = toolList[i];
    connect(btn, &QPushButton::toggled, this, [this, i](bool isToggle) {
      if (isToggle) {
        this->handleSwitchTool(i);
      }
    });
    layout->addWidget(btn);
  }
}

void MainStageSideToolBar::setPositionFromRect(const QRect& rect) {
  this->move(rect.x(), rect.y() + 20);
  this->resize(50, rect.height() - 40);
}

int MainStageSideToolBar::getCurrentTool() const {
  return this->currentBtnIndex;
}

void MainStageSideToolBar::setEnableTool(int index, bool enable) {
  auto tool = toolList[index];
  if (!enable) {
    // if disabled tool is current tool set to 0
    // which is the tool will never disabled
    if (index == 0) {
      return;
    }
    if (index == currentBtnIndex) {
      this->handleSwitchTool(0);
    }
    tool->setVisible(false);
  } else {
    tool->setVisible(true);
  }
}
}  // namespace views
