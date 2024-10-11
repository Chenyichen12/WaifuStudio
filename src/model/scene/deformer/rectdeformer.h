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
  /**
   * @brief Create a Operate Point, remember to set the point data index later
   *
   * @param p point position
   * @return OperatePoint* operate point
   */
  OperatePoint* createOperatePoint(const QPointF& p);

  /**
   * @brief emit the deformer command
   * 
   * @param newPoints new points positions
   * @param isStart is the start drag
   */
  void handlePointShouldMove(const QList<QPointF>& newPoints, bool isStart);
  unsigned int row_;
  unsigned int column_;
};

}  // namespace WaifuL2d