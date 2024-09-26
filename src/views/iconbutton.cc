#include "iconbutton.h"
#include <QPainter>
#include <QMouseEvent>

namespace views {
void IconButton::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  if (this->selected) {
    painter.setBrush(QColor("#A2C3E8"));
  } else {
    painter.setBrush(Qt::white);
  }

  painter.drawRect(this->rect());
  auto center = rect().center();
  auto height = icon.height();
  QRect imageRect(center.x() - height / 2, center.y() - height / 2, height,
                  height);
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

void IconButton::setIconLength(int length) {
  icon = icon.scaledToHeight(length, Qt::SmoothTransformation);
  update();
}

void IconButton::setSelect(bool select) {
  this->selected = select;
  update();
}
}