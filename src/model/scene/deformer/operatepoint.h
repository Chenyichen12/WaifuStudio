//
// Created by chenyichen on 2024/9/19.
//

#pragma once
#include <QGraphicsEllipseItem>
namespace WaifuL2d {
class PointOperator {
 public:
  virtual void pointSelectedChange(int id) = 0;
  virtual void pointShouldMove(int index, const QPointF& point, bool isEnd) = 0;
};
class OperatePoint : public QGraphicsEllipseItem {
  PointOperator* oper;
  int id;

  bool isMove = false;
 public:
  explicit OperatePoint(int id, PointOperator* oper, QGraphicsItem* parent = nullptr);
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
};
}  // namespace WaifuL2d
