//
// Created by chenyichen on 8/5/24.
//
#pragma once
#include <QGraphicsItem>
#include <QOpenGLFunctions>
class Sprite: public QGraphicsItem, protected QOpenGLFunctions{
 private:
 public:
  explicit Sprite(QGraphicsItem *parent = nullptr);
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
};

