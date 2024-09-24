//
// Created by chenyichen on 2024/9/19.
//
#pragma once
#include "../abstractdeformer.h"
#include "operatepoint.h"

namespace WaifuL2d {
class Mesh;

class MeshDeformer : public AbstractDeformer {
private:
  Mesh* mesh;
  QList<OperatePoint*> operatePoints;

  /**
   * record one drag state of the rectangle
   */
  OperateRectangle* operateRect;

  struct {
    QList<QPointF> startOpPoints;
    QList<int> opPointIndexes;
  } rectMoveState, rectRotateState;

  /**
   * depend on selected point, if select point is empty or only has one
   * it will assign all points
   * @param resultPoint assign result point
   * @param resultIndexes assign result indexes
   */
  void getOperatePoints(QList<QPointF>& resultPoint,
                        QList<int>& resultIndexes) const;


  bool deformerSelect = false;

  void handlePointShouldMove(const QList<QPointF>& newPoints, bool isStart);
  void upDateOperateRect();

  QList<int> getSelectedIndex() const;

public:
  explicit MeshDeformer(Mesh* mesh, QGraphicsItem* parent = nullptr);

  QList<QPointF> getScenePoints() const override;
  void setScenePoints(const QList<QPointF>& points) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  int type() const override { return MeshDeformerType; }
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = nullptr) override;
  QRectF boundingRect() const override;
  void setDeformerSelect(bool select) override;
  bool isDeformerSelected() const override { return deformerSelect; }
  bool isHitDeformer(const QPointF& point) const override;

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                      const QVariant& value) override;
};
} // namespace WaifuL2d