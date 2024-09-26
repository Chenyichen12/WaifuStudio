#include "mesheditor.h"
#include "operatepoint.h"
#include <QPainter>

namespace WaifuL2d {
MeshEditor::MeshEditor(QGraphicsItem* parent): MeshEditor({}, {}, parent) {
}

void MeshEditor::setHandleRect(const QRectF& rect) {
  this->handleRect = rect;
  prepareGeometryChange();
}

QRectF MeshEditor::boundingRect() const { return handleRect; }

void MeshEditor::paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget) {
  auto pen = QPen();
  pen.setWidth(1 / painter->transform().m11());
  pen.setColor(Qt::gray);

  painter->setPen(pen);
  for (const auto& edge : allEdges) {
    painter->drawLine(points[edge.v1()]->pos(), points[edge.v2()]->pos());
  }

  pen.setColor(Qt::black);
  painter->setPen(pen);

  for (const auto& edge : fixedEdges) {
    painter->drawLine(points[edge.v1()]->pos(), points[edge.v2()]->pos());
  }
}

OperatePoint* MeshEditor::createPoint(const QPointF& pos) {
  auto* point = new OperatePoint(this);
  point->setRadius(4);
  point->setPos(pos);
  point->pointShouldMove = [this](const QPointF& pos, bool isStart,
                                  const QVariant& data) {
    handlePointShouldMove(pos, isStart, data);
  };
  return point;
}

void MeshEditor::updatePoints(const QList<QPointF>& pointPositions) {
  if (points.size() == pointPositions.size()) {
    for (int i = 0; i < pointPositions.size(); i++) {
      points[i]->setPos(pointPositions[i]);
    }
    return;
  }

  if (points.size() < pointPositions.size()) {
    for (size_t i = 0; i < points.size(); i++) {
      points[i]->setPos(pointPositions[i]);
    }

    for (size_t i = points.size(); i < pointPositions.size(); i++) {
      auto* point = createPoint(pointPositions[i]);
      points.push_back(point);
      point->data = i;
    }
    return;
  }

  if (points.size() > pointPositions.size()) {
    for (size_t i = 0; i < pointPositions.size(); i++) {
      points[i]->setPos(pointPositions[i]);
    }

    for (size_t i = pointPositions.size(); i < points.size(); i++) {
      delete points[i];
    }
    points.resize(pointPositions.size());
    return;
  }
}

void MeshEditor::
handlePointShouldMove(const QPointF& pos, bool isStart, const QVariant& data) {
  auto index = data.toInt();
  qDebug() << pos;
}

MeshEditor::MeshEditor(const QList<QPointF>& initPoints,
                       const QList<unsigned int>& initIncident,
                       QGraphicsItem* parent)
  : QGraphicsObject(parent) {
  updatePoints(initPoints);
  for (int i = 0; i < initIncident.size(); i += 3) {
    CDT::Edge e1 = {initIncident[i], initIncident[i + 1]};
    CDT::Edge e2 = {initIncident[i + 1], initIncident[i + 2]};
    CDT::Edge e3 = {initIncident[i + 2], initIncident[i]};

    fixedEdges.insert(e1);
    fixedEdges.insert(e2);
    fixedEdges.insert(e3);
  }
  this->allEdges = fixedEdges;
}
}