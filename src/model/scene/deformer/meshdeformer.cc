//
// Created by chenyichen on 2024/9/19.
//

#include "meshdeformer.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "../mesh/mesh.h"
#include "../meshmathtool.hpp"
namespace WaifuL2d {
MeshDeformer::MeshDeformer(WaifuL2d::Mesh *mesh, QGraphicsItem *parent)
    : AbstractDeformer(parent), mesh(mesh) {
  setVisible(false);
  const auto &pointList = mesh->getPos();
  for (int i = 0; i < pointList.size(); i++) {
    OperatePoint *op = new OperatePoint(i, this, this);
    operatePoints.push_back(op);
    op->setRect(-smallSize, -smallSize, 2 * smallSize, 2 * smallSize);
    op->setPos(pointList[i]);
  }
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
  auto pen = QPen();
  pen.setWidth(smallSize / 5);
  pen.setColor(Qt::black);
  painter->setPen(pen);
  const auto &incident = mesh->getIncident();
  const auto &pList = mesh->getPos();
  for (int i = 0; i < incident.size(); i += 3) {
    const auto &p1 = pList[incident[i]];
    const auto &p2 = pList[incident[i + 1]];
    const auto &p3 = pList[incident[i + 2]];
    painter->drawLine(p1, p2);
    painter->drawLine(p2, p3);
    painter->drawLine(p3, p1);
  }
}
QRectF MeshDeformer::boundingRect() const {
  const auto &pos = getScenePoints();
  return MeshMathTool<QPointF>::calculateBoundRect(pos.data(), pos.size())
      .marginsAdded(QMarginsF(smallSize, smallSize, smallSize, smallSize));
}

void MeshDeformer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  event->ignore();
}

void MeshDeformer::setDeformerSelect(bool select) {
  if (!select) {
    this->setVisible(false);
    return;
  }
  if (this->isEnabled()) {
    this->setVisible(select);
  }
}
void MeshDeformer::pointSelectedChange(int id) { qDebug() << id; }
void MeshDeformer::pointShouldMove(int index, const QPointF &point) {
  qDebug() << point;
}
void MeshDeformer::setSmallSize(int size) {
  smallSize = size;
  for (auto &op : operatePoints) {
    op->setRect(-size, -size, 2 * size, 2 * size);
  }
}
}  // namespace WaifuL2d