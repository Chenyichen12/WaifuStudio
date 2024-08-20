#include "mainstagetopbar.h"

#include <QHBoxLayout>

#include "QPushButton"

namespace views {
MainStageTopBar::MainStageTopBar(QWidget* parent) : QWidget(parent) {
  auto layout = new QHBoxLayout(this);
  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

  this->editModeBtn = new QPushButton(tr("edit mesh"), this);
  editModeBtn->setCheckable(true);
  editModeBtn->setChecked(false);
  layout->addWidget(editModeBtn);

  layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
  this->setLayout(layout);

  connect(editModeBtn, &QPushButton::toggled, this, [this](bool isToggled) {
    if (isToggled) {
      emit this->enterEditMode();
    } else {
      emit this->leaveEditMode();
    }
  });
}
}  // namespace views
