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
MeshDeformer::MeshDeformer(WaifuL2d::Mesh* mesh, QGraphicsItem* parent)
  : AbstractDeformer(parent), mesh(mesh) {
  const auto& pointList = mesh->getPos();
  for (int i = 0; i < pointList.size(); i++) {
    OperatePoint* op = new OperatePoint(this);
    op->data = i;
    operatePoints.push_back(op);
    op->setPos(pointList[i]);
    op->pointShouldMove = [this](const QPointF& point, bool isStart,
                                 const QVariant& data) {
      QList<QPointF> newPoints = this->getScenePoints();
      auto index = data.toInt();
      newPoints[index] = point;
      this->handlePointShouldMove(newPoints, isStart);
    };
    op->pointSelectedChange = [this](bool isSelected, const QVariant& data) {
      this->upDateOperateRect();
    };
  }
  operateRect = new OperateRectangle(this);
  operateRect->setRect(MeshDeformer::boundingRect());
  operateRect->rectShouldResize = [this](const QRectF& startRect,
                                         const QRectF& newRect, bool xFlip,
                                         bool yFlip, bool isStart,
                                         const QVariant& data) {
    if (isStart) {
      this->getOperatePoints(rectMoveState.startOpPoints,
                             rectMoveState.opPointIndexes);
    }

    auto newPoints = this->rectMoveState.startOpPoints;
    MeshMathTool<QPointF>::resizePointInBound(startRect,
                                              newRect, newPoints.data(),
                                              newPoints.size(), xFlip, yFlip);

    auto formatResult = this->getScenePoints();
    for (int i = 0; i < this->rectMoveState.opPointIndexes.size(); i++) {
      formatResult[this->rectMoveState.opPointIndexes[i]] = newPoints[i];
    }
    this->handlePointShouldMove(formatResult, isStart);
  };
  operateRect->rectShouldRotate = [this](const QPointF& rotationCenter,
                                         qreal angle, bool isStart,
                                         const QVariant& data) {
    if (isStart) {
      this->getOperatePoints(rectRotateState.startOpPoints,
                             rectRotateState.opPointIndexes);
    }

    auto newPoints = this->rectRotateState.startOpPoints;
    MeshMathTool<QPointF>::rotatePoints(angle,
                                        rotationCenter,
                                        newPoints.data(),
                                        newPoints.size());
    auto formatResult = this->getScenePoints();
    for (int i = 0; i < this->rectRotateState.opPointIndexes.size(); i++) {
      formatResult[this->rectRotateState.opPointIndexes[i]] = newPoints[i];
    }
    handlePointShouldMove(formatResult, isStart);
  };

  MeshDeformer::setDeformerSelect(false);
}

void MeshDeformer::upDateOperateRect() {
  auto selectIndex = getSelectedIndex();
  if (selectIndex.empty() || selectIndex.size() == 1) {
    operateRect->setRect(boundingRect());
  } else {
    auto points = QList<QPointF>();
    for (auto& index : selectIndex) {
      points.push_back(operatePoints[index]->pos());
    }
    auto bound =
        MeshMathTool<QPointF>::calculateBoundRect(points.data(), points.size());
    operateRect->setRect(bound);
    update();
  }
}

QList<QPointF> MeshDeformer::getScenePoints() const { return mesh->getPos(); }

void MeshDeformer::setScenePoints(const QList<QPointF>& points) {
  for (int i = 0; i < mesh->getPos().size(); i++) {
    mesh->changeVertexPos(points[i], i);
    operatePoints[i]->setPos(points[i]);
  }
  upDateOperateRect();
  mesh->upDateBuffer();
  update();
}

QPointF MeshDeformer::scenePointToLocal(const QPointF& point) {
  // TODO: maybe need more
  return point;
}

void MeshDeformer::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget) {
  if (!deformerSelect) {
    return;
  }
  auto pen = QPen();
  pen.setWidth(1 / painter->transform().m11());
  pen.setColor(Qt::black);
  painter->setPen(pen);
  const auto& incident = mesh->getIncident();
  const auto& pList = mesh->getPos();
  for (int i = 0; i < incident.size(); i += 3) {
    const auto& p1 = pList[incident[i]];
    const auto& p2 = pList[incident[i + 1]];
    const auto& p3 = pList[incident[i + 2]];
    painter->drawLine(p1, p2);
    painter->drawLine(p2, p3);
    painter->drawLine(p3, p1);
  }
}

QRectF MeshDeformer::boundingRect() const {
  const auto& pos = getScenePoints();
  auto bound =
      MeshMathTool<QPointF>::calculateBoundRect(pos.data(), pos.size());
  return bound;
}

void MeshDeformer::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  event->ignore();
}

void MeshDeformer::setDeformerSelect(bool select) {
  deformerSelect = select;
  for (auto& op : operatePoints) {
    op->setVisible(deformerSelect);
  }
  operateRect->setVisible(deformerSelect);
  update();
}

QVariant MeshDeformer::itemChange(QGraphicsItem::GraphicsItemChange change,
                                  const QVariant& value) {
  if (change == QGraphicsItem::ItemVisibleChange) {
    if (!value.toBool()) {
      this->setDeformerSelect(false);
    }
    this->mesh->setVisible(value.toBool());
  }
  if (change == QGraphicsItem::ItemZValueChange) {
    this->mesh->setZValue(value.toDouble());
  }
  return QGraphicsItem::itemChange(change, value);
}

bool MeshDeformer::isHitDeformer(const QPointF& point) const {
  return mesh->hitTest(point);
}

void MeshDeformer::getOperatePoints(QList<QPointF>& resultPoint,
                                    QList<int>& resultIndexes) const {
  resultIndexes.clear();
  resultIndexes = this->getSelectedIndex();
  if (resultIndexes.empty() || resultIndexes.size() == 1) {
    resultIndexes.clear();
    for (int i = 0; i < operatePoints.size(); i++) {
      resultIndexes.push_back(i);
    }
  }

  resultPoint.clear();
  for (auto& index : resultIndexes) {
    resultPoint.push_back(
        this->operatePoints[index]->pos());
  }
}

void MeshDeformer::handlePointShouldMove(const QList<QPointF>& newPoints,
                                         bool isStart) {
  auto command = std::make_shared<DeformerCommand>();
  command->info.oldPoints = this->getScenePoints();
  command->info.newPoints = newPoints;

  command->info.isStart = isStart;
  command->info.target = this;
  auto sc = static_cast<MainStageScene*>(scene());
  sc->emitDeformerCommand(command);
}

QList<int> MeshDeformer::getSelectedIndex() const {
  QList<int> result;
  for (int i = 0; i < operatePoints.size(); i++) {
    if (operatePoints[i]->isSelected()) {
      result.push_back(i);
    }
  }
  return result;
}

QList<unsigned int> MeshDeformer::getMeshIncident() const {
  return mesh->getIncident();
}
} // namespace WaifuL2d