//
// Created by chenyichen on 2024/9/19.
//
#include "operatepoint.h"

#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
namespace WaifuL2d {
OperatePoint::OperatePoint(int id, WaifuL2d::PointOperator *oper,
                           QGraphicsItem *parent)
    : oper(oper), QGraphicsRectItem(parent), id(id) {
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
  this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
  this->setBrush(QBrush(Qt::white));
  this->setRect(-radius, -radius, 2 * radius, 2 * radius);
}
void OperatePoint::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsRectItem::mousePressEvent(event);
}
void OperatePoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  oper->pointShouldMove(id, event->scenePos(), false);
  isMove = true;
}

void OperatePoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (isMove) {
    oper->pointShouldMove(id, event->scenePos(), true);
    isMove = false;
  }
}

void OperatePoint::setRadius(int r) {
  radius = r;
  this->setRect(-r, -r, 2 * r, 2 * r);
}
QVariant OperatePoint::itemChange(QGraphicsItem::GraphicsItemChange change,
                                  const QVariant &value) {
  if (change == QGraphicsItem::ItemSelectedChange) {
    oper->pointSelectedChange(id);
  }
  return QGraphicsItem::itemChange(change, value);
}

void OperatePoint::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget) {
  auto pen = QPen();
  pen.setWidth(rect().width() / 10);
  pen.setColor(Qt::black);
  painter->setPen(pen);
  if (this->isSelected()) {
    painter->setBrush(Qt::red);
  } else {
    painter->setBrush(Qt::white);
  }

  painter->drawEllipse(this->rect());
}
}  // namespace WaifuL2d
