//
// Created by chenyichen on 8/5/24.
//

#include "sprite.h"

#include "QPainter"
#include "QStyleOptionGraphicsItem"
Sprite::Sprite(QGraphicsItem *parent) : QGraphicsItem(parent) {}
QRectF Sprite::boundingRect() const { return QRectF(0, 0, 100, 100); }
void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) {
  painter->setBrush(QBrush(Qt::red));
  painter->drawRect(option->rect);
}
