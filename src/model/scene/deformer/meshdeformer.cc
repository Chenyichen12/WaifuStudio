//
// Created by chenyichen on 2024/9/19.
//

#include "meshdeformer.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "../deformercommand.h"
#include "../mainstagescene.h"
#include "../mesh/mesh.h"
#include "../meshmathtool.hpp"
namespace WaifuL2d {

MeshDeformer::MeshDeformer(WaifuL2d::Mesh *mesh, QGraphicsItem *parent)
    : AbstractDeformer(parent), mesh(mesh) {
  setVisible(true);
  const auto &pointList = mesh->getPos();
  for (int i = 0; i < pointList.size(); i++) {
    OperatePoint *op = new OperatePoint(i, this, this);
    operatePoints.push_back(op);
    op->setPos(pointList[i]);
  }
}
QList<QPointF> MeshDeformer::getScenePoints() const { return mesh->getPos(); }
void MeshDeformer::setScenePoints(const QList<QPointF> &points) {
  for (int i = 0; i < mesh->getPos().size(); i++) {
    mesh->changeVertexPos(points[i], i);
    operatePoints[i]->setPos(points[i]);
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
  pen.setWidth(1 / painter->transform().m11());
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
  return MeshMathTool<QPointF>::calculateBoundRect(pos.data(), pos.size());
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
void MeshDeformer::pointShouldMove(int index, const QPointF &point,
                                   bool isEnd) {
  auto command = std::make_shared<DeformerCommand>(this);
  command->oldPoints = this->getScenePoints();
  command->newPoints = this->getScenePoints();
  command->newPoints[index] = point;
  command->setEnd(isEnd);
  auto sc = static_cast<MainStageScene *>(scene());
  sc->emitDeformerCommand(command);
}

QVariant MeshDeformer::itemChange(QGraphicsItem::GraphicsItemChange change,
                                  const QVariant &value) {
  if (change == QGraphicsItem::ItemEnabledChange) {
    if(!value.toBool()){
      this->setDeformerSelect(false);
    }
    this->mesh->setVisible(value.toBool());
  }
  return QGraphicsItem::itemChange(change, value);
}
}  // namespace WaifuL2d