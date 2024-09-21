//
// Created by chenyichen on 2024/9/21.
//

#pragma once
#include <QGraphicsItem>

#include "operatepoint.h"
namespace WaifuL2d {
class RectangleTool : public QGraphicsItem, public PointOperator {
 private:
  std::array<OperatePoint *, 9> operatePoints;
  QRectF rect;

 public:
  explicit RectangleTool(QGraphicsItem *parent = nullptr);
  ~RectangleTool() override;
  void pointSelectedChange(int id) override;
  void pointShouldMove(int index, const QPointF &point, bool isEnd) override;
  void setRect(const QRectF &rect);
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
};
}  // namespace WaifuL2d
