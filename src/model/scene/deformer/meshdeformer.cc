//
// Created by chenyichen on 2024/9/19.
//

#include "meshdeformer.h"

#include <QPainter>

#include "../mesh/mesh.h"
#include "../meshmathtool.hpp"
namespace WaifuL2d {
MeshDeformer::MeshDeformer(WaifuL2d::Mesh *mesh, QGraphicsItem *parent)
    : AbstractDeformer(parent), mesh(mesh) {
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}
QList<QPointF> MeshDeformer::getScenePoints() const { return mesh->getPos(); }
void MeshDeformer::setScenePoint(const QPointF &point, int index) {
  Q_ASSERT(index < mesh->getPos().size() && index >= 0);
  mesh->changeVertexPos(point, index);
  mesh->upDateBuffer();
  update();
}
void MeshDeformer::setScenePoints(const QList<QPointF> &points) {
  for (int i = 0; i < mesh->getPos().size(); i++) {
    mesh->changeVertexPos(points[i], i);
  }
  mesh->upDateBuffer();
  update();
}
QPointF MeshDeformer::scenePointToLocal(const QPointF &point) {
  // TODO: maybe need more
  return point;
}
void MeshDeformer::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget) {
  if (!isSelected()) {
    return;
  }
  const auto &pos = getScenePoints();
  painter->setBrush(Qt::red);
  for (auto &&p : pos) {
    painter->drawEllipse(p, 20, 20);
  }
}
QRectF MeshDeformer::boundingRect() const {
  const auto &pos = getScenePoints();
  return MeshMathTool<QPointF>::calculateBoundRect(pos.data(), pos.size());
}
void MeshDeformer::selectPoint(int index) {
  selectedPoints.append(index);
  update();
}
void MeshDeformer::clearSelection() {
  selectedPoints.clear();
  update();
}
void MeshDeformer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
//    QGraphicsItem::mousePressEvent(event);
}
void MeshDeformer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
//    QGraphicsItem::mouseMoveEvent(event);
}
void MeshDeformer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
//    QGraphicsItem::mouseReleaseEvent(event);
}

}  // namespace WaifuL2d