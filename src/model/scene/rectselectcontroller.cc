#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>

#include "scenecontroller.h"

namespace Scene {

QRectF RectSelectController::boundingRect() const {
  return boundRect.marginsAdded(QMarginsF(padding, padding, padding, padding));
}

void RectSelectController::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget) {
  if (this->boundRect == QRectF()) {
    return;
  }

  painter->setBrush(QBrush(Qt::transparent));
  auto pen = QPen(Qt::red);
  pen.setWidth(lineWidth);
  painter->setPen(pen);
  painter->drawRect(boundingRect().marginsRemoved(
      {lineWidth / 2.0, lineWidth / 2.0, lineWidth / 2.0, lineWidth / 2.0}));
}

QRectF RectSelectController::boundRectFromPoints(
    const std::vector<QPointF>& list) {
  if (list.empty() || list.size() == 1) {
    return {};
  }
  float leftMost = FLT_MAX;
  float topMost = FLT_MAX;
  float rightMost = FLT_MIN;
  float bottomMost = FLT_MIN;
  for (const auto& scenePoint : list) {
    if (scenePoint.x() > rightMost) {
      rightMost = scenePoint.x();
    }
    if (scenePoint.x() < leftMost) {
      leftMost = scenePoint.x();
    }
    if (scenePoint.y() > bottomMost) {
      bottomMost = scenePoint.y();
    }
    if (scenePoint.y() < topMost) {
      topMost = scenePoint.y();
    }
  }
  return {QPointF(leftMost, topMost), QPointF(rightMost, bottomMost)};
}

void RectSelectController::setBoundRect(const QRectF& rect) {
  this->boundRect = rect;
  update();
}

QRectF RootController::boundingRect() const {
  return QRectF(0, 0, width, height);
}

void RectSelectController::setPadding(double padding) {
  this->padding = padding;
  update();
}

void RectSelectController::setLineWidth(double lineWidth) {
  this->lineWidth = lineWidth;
  update();
}

RectSelectController::RectSelectController(QGraphicsItem* parent)
    : QGraphicsItem(parent) {
  setAcceptHoverEvents(true);
}

void RectSelectController::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
  if (ifHitRectBorder(event->scenePos())) {
    setCursor(QCursor(Qt::SizeAllCursor));
    this->lastMovePoint = event->scenePos();
  } else {
    setCursor(QCursor(Qt::ArrowCursor));
  }
}

void RectSelectController::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (this->ifHitRectBorder(event->scenePos())) {
    event->accept();
    this->lastMovePoint = event->scenePos();
  } else {
    event->ignore();
  }
}

void RectSelectController::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  this->moveCallBack(lastMovePoint, event->scenePos());
  auto delta = event->scenePos() - lastMovePoint;
  auto newBound = this->boundRect;
  newBound.translate(delta);
  this->setBoundRect(newBound);

  lastMovePoint = event->scenePos();
}

void RectSelectController::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  lastMovePoint.setX(0);
  lastMovePoint.setY(0);
  this->moveEndCallBack();
}

bool RectSelectController::ifHitRectBorder(const QPointF& p) const {
  const auto& rect = boundingRect();
  const auto& insideRect = rect.marginsRemoved(
      QMarginsF(lineWidth, lineWidth, lineWidth, lineWidth));
  return rect.contains(p) && !insideRect.contains(p);
}
}  // namespace Scene