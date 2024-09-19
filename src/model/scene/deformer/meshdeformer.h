//
// Created by chenyichen on 2024/9/19.
//
#pragma once
#include "../abstractdeformer.h"
namespace WaifuL2d {
class Mesh;
class MeshDeformer: public AbstractDeformer{
 private:
  Mesh* mesh;
  QList<unsigned int> selectedPoints;
 public:
  explicit MeshDeformer(Mesh* mesh, QGraphicsItem* parent = nullptr);

  QList<QPointF> getScenePoints() const override;
  void setScenePoint(const QPointF& point, int index) override;
  void setScenePoints(const QList<QPointF>& points) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  int type() const override{return MeshDeformerType;}
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

 public:
  void selectPoint(int index);
  void clearSelection();

};

}  // namespace WaifuL2d

