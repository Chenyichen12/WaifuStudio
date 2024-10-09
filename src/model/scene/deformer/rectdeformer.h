#pragma once
#include "model/scene/abstractdeformer.h"
#include "operatepoint.h"
namespace WaifuL2d {
class RectDeformer : public AbstractDeformer {
 public:
  explicit RectDeformer(const QRectF& initRect, unsigned int row = 3,
                        unsigned int column = 3,
                        QGraphicsItem* parent = nullptr);
  QList<QPointF> getScenePoints() const override;
  void setScenePoints(const QList<QPointF>& points) override;
  QPointF scenePointToLocal(const QPointF& point) const override;
  void setDeformerSelect(bool select) override;
  bool isDeformerSelected() const override;
  int type() const override { return AbstractDeformer::RectDeformerType; }
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

 private:
  QList<OperatePoint*> operator_points_;
  OperatePoint* createOperatePoint(const QPointF& p);
  unsigned int row_;
  unsigned int column_;
};

}  // namespace WaifuL2d