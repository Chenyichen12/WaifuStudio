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

  /**
   * will call the scene emit deformer command to refresh the new points
   * @param newPoints point position
   * @param isStart if is start drag
   */
  void handlePointShouldMove(const QList<QPointF>& newPoints, bool isStart);
  /**
   * the mesh deformer has a rect of selection points
   * update it if selection points have changed
   */
  void upDateOperateRect();

public:
  explicit MeshDeformer(Mesh* mesh, QGraphicsItem* parent = nullptr);

  /**
   * @return selected point position
   */
  QList<QPointF> getScenePoints() const override;
  /**
   * @return selected points index
   */
  QList<int> getSelectedIndex() const;
  QList<unsigned int> getMeshIncident() const;

  void setScenePoints(const QList<QPointF>& points) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  int type() const override { return MeshDeformerType; }
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = nullptr) override;
  QRectF boundingRect() const override;
  void setDeformerSelect(bool select) override;
  bool isDeformerSelected() const override { return deformerSelect; }
  bool isHitDeformer(const QPointF& point) const override;

  /**
   * change the mesh struct
   * call it after finish the edit mode
   * will push the undo command to main undo stack
   * @param points 
   * @param incident 
   */
  void handleShouldChangeMeshStruct(const QList<QPointF>& points,
                                    const QList<unsigned int>& incident);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                      const QVariant& value) override;
};
} // namespace WaifuL2d