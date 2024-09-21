//
// Created by chenyichen on 2024/9/21.
//

#include "rectangletool.h"

#include <QPainter>
namespace WaifuL2d {
class RectangleOperatePoint : public OperatePoint {
 public:
  RectangleOperatePoint(int id, PointOperator *oper, QGraphicsItem *parent = nullptr)
      : OperatePoint(id, oper, parent) {}
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget = nullptr) override {
    auto brush = QBrush(Qt::green);
    painter->setBrush(brush);
    painter->drawRect(this->rect());
  }
};
}  // namespace WaifuL2d
void WaifuL2d::RectangleTool::pointSelectedChange(int id) {}
void WaifuL2d::RectangleTool::pointShouldMove(int index, const QPointF &point,
                                              bool isEnd) {}
QRectF WaifuL2d::RectangleTool::boundingRect() const {
  return rect.marginsAdded({50, 50, 50, 50});
}
void WaifuL2d::RectangleTool::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *widget) {
  if (boundingRect() == QRectF()) {
    return;
  }
  auto pen = QPen();
  pen.setWidth(2);
  pen.setColor(Qt::green);
  painter->setPen(pen);
}
WaifuL2d::RectangleTool::RectangleTool(QGraphicsItem *parent)
    : QGraphicsItem(parent) {
  for (int i = 0; i < 9; i++) {
    operatePoints[i] = new RectangleOperatePoint(i, this, this);
  }
//  this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}
void WaifuL2d::RectangleTool::setRect(const QRectF &rect) {
  this->rect = {0,0, rect.width(),rect.height()};
  auto points = boundingRect().topLeft();
  auto width = boundingRect().width();
  auto height = boundingRect().height();

  operatePoints[0]->setPos(points);
  operatePoints[1]->setPos(points + QPointF(width / 2, 0));
  operatePoints[2]->setPos(points + QPointF(width, 0));
  operatePoints[3]->setPos(points + QPointF(0, height / 2));
  operatePoints[4]->setPos(points + QPointF(width / 2, height / 2));
  operatePoints[5]->setPos(points + QPointF(width, height / 2));
  operatePoints[6]->setPos(points + QPointF(0, height));
  operatePoints[7]->setPos(points + QPointF(width / 2, height));
  operatePoints[8]->setPos(points + QPointF(width, height));

  setPos(rect.topLeft());
  prepareGeometryChange();
}
WaifuL2d::RectangleTool::~RectangleTool() {
  for (auto &point : operatePoints) {
    delete point;
  }
}
