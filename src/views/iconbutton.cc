#include "iconbutton.h"
#include <QPainter>
#include <QMouseEvent>

namespace views {
void IconButton::paintEvent(QPaintEvent* event) {
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

void IconButton::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);
}

void IconButton::mouseReleaseEvent(QMouseEvent* event) {
  QWidget::mouseReleaseEvent(event);
  if (this->rect().contains(event->pos())) {
    emit clicked();
  }
}

IconButton::IconButton(QWidget* parent) : QWidget(parent) {
  this->selected = false;
  setMinimumWidth(30);
  setMinimumHeight(30);
}

void IconButton::setIcon(const QString& iconPath) {
  icon = QImage(iconPath);
  icon = icon.scaledToHeight(20, Qt::SmoothTransformation);
  update();
}

void IconButton::setSelect(bool select) {
  this->selected = select;
  update();
}
}