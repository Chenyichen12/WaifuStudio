//
// Created by chenyichen on 2024/9/19.
//
#pragma once
#include "../abstractdeformer.h"
#include "operatepoint.h"
namespace WaifuL2d {
class Mesh;
class MeshDeformer: public AbstractDeformer, public PointOperator{
 private:
  Mesh* mesh;
  QList<unsigned int> selectedPoints;
  QList<OperatePoint*> operatePoints;
  int smallSize = 10;
 public:
  explicit MeshDeformer(Mesh* mesh, QGraphicsItem* parent = nullptr);

  QList<QPointF> getScenePoints() const override;
  void setScenePoint(const QPointF& point, int index) override;
  void setScenePoints(const QList<QPointF>& points) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  int type() const override{return MeshDeformerType;}
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

  void setDeformerSelect(bool select) override;

  void pointSelectedChange(int id) override;
  void pointShouldMove(int index, const QPointF& point) override;

  void setSmallSize(int size) override;
 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

}  // namespace WaifuL2d

