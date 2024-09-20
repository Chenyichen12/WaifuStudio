//
// Created by chenyichen on 2024/9/19.
//
#pragma once
#include "../abstractdeformer.h"
#include "operatepoint.h"
namespace WaifuL2d {
class Mesh;
class MeshDeformer : public AbstractDeformer, public PointOperator {
 private:
  Mesh* mesh;
  QList<OperatePoint*> operatePoints;

  bool deformerSelect = false;

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
  void pointSelectedChange(int id) override;
  void pointShouldMove(int index, const QPointF& point, bool isEnd) override;

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                      const QVariant& value) override;
};

}  // namespace WaifuL2d
