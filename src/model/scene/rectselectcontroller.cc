#include "rectselectcontroller.h"

#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>

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
      {lineWidth, lineWidth, lineWidth, lineWidth}));

  auto controllerPen = QPen(Qt::transparent);
  controllerPen.setWidth(0);
  painter->setPen(controllerPen);
  painter->setBrush(QBrush(Qt::red));
  for (int i = 0; i < 9; i++) {
    auto controllerBound =
        this->getHandlerVisibleRect(static_cast<HandledState>(i));
    painter->drawRect(controllerBound);
  }
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
  // table of the cursor
  static std::array<QCursor, 10> cursorMap = {
      QCursor(Qt::SizeFDiagCursor), QCursor(Qt::SizeVerCursor),
      QCursor(Qt::SizeBDiagCursor), QCursor(Qt::SizeHorCursor),
      QCursor(Qt::SizeAllCursor),   QCursor(Qt::SizeHorCursor),
      QCursor(Qt::SizeBDiagCursor), QCursor(Qt::SizeVerCursor),
      QCursor(Qt::SizeFDiagCursor), QCursor(Qt::ArrowCursor)};

  auto which = ifHitControllerPoint(event->scenePos());
  setCursor(cursorMap[which]);
}

void RectSelectController::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  auto handle = ifHitControllerPoint(event->scenePos());
  if (handle != NONE) {
    event->accept();
    this->lastMovePoint = event->scenePos();
    this->startPoint = event->scenePos();
    this->dragState = handle;
    this->rectStartMove(startPoint);
  } else {
    event->ignore();
  }
}

void RectSelectController::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  this->rectMoving(lastMovePoint, event->scenePos());
  if (ifAutoMoveUpdate) {
    auto delta = event->scenePos() - lastMovePoint;
    auto newBound = this->boundRect;
    newBound.translate(delta);
    this->setBoundRect(newBound);
  }
  lastMovePoint = event->scenePos();
}

void RectSelectController::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  lastMovePoint.setX(0);
  lastMovePoint.setY(0);
  this->endPoint = event->scenePos();
  this->dragState = NONE;
  this->rectEndMove(startPoint, endPoint);
}


RectSelectController::HandledState RectSelectController::ifHitControllerPoint(
    const QPointF& p) const {
  for (int i = 0; i < 9; i++) {
    auto controllerRect = getHandlerVisibleRect(static_cast<HandledState>(i));
    if (controllerRect.contains(p)) {
      return static_cast<HandledState>(i);
    }
  }
  return NONE;
}

QRectF RectSelectController::getHandlerVisibleRect(HandledState state) const {
  if (state == NONE) {
    return {};
  }
  auto dePoint = QPointF(lineWidth, lineWidth);
  auto centerPoint = getHandlerHitPoint(state);
  centerPoint -= dePoint;
  return {centerPoint.x(), centerPoint.y(), lineWidth * 2, lineWidth * 2};
}

QPointF RectSelectController::getHandlerHitPoint(HandledState state) const {
  auto bound = boundingRect().marginsRemoved(QMarginsF(lineWidth,lineWidth,lineWidth,lineWidth));

  float xMid = bound.left() / 2 + bound.right() / 2;
  float yMid = bound.top() / 2 + bound.bottom() / 2;
  std::array<QPointF, 10> map = {
      bound.topLeft(),     QPointF(xMid, bound.top()),
      bound.topRight(),    QPointF(bound.left(), yMid),
      QPointF(xMid, yMid), QPointF(bound.right(), yMid),
      bound.bottomLeft(),  QPointF(xMid, bound.bottom()),
      bound.bottomRight(), QPointF()};

  return map[state];
}
}  // namespace Scene