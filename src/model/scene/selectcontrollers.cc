#include "selectcontrollers.h"

#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <cfloat>


namespace Scene {

// ---------------------AbstractSelectController---------------------
AbstractSelectController::AbstractSelectController(QGraphicsItem* parent)
    : QGraphicsItem(parent) {}

void AbstractSelectController::setBoundRect(
    const std::vector<QPointF>& pointList) {
  auto bound_rect = AbstractSelectController::boundRectFromPoints(pointList);
  this->setBoundRect(bound_rect);
}

void AbstractSelectController::setBoundRect(const QRectF& rect) {
  // when there is no rect of the bound should set it to invisible
  // set visible when the bound is not null
  if (rect == QRectF()) {
    this->setVisible(false);
  } else {
    this->setVisible(true);
  }
  this->boundRect = rect;
}

QRectF AbstractSelectController::boundRectFromPoints(
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

// ---------------------RectSelectController---------------------
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

void RectSelectController::setBoundRect(const QRectF& rect) {
  AbstractSelectController::setBoundRect(rect);
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
    : AbstractSelectController(parent) {
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
  auto bound = boundingRect().marginsRemoved(
      QMarginsF(lineWidth, lineWidth, lineWidth, lineWidth));

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

// ---------------------RotationSelectController---------------------


QPainterPath RotationSelectController::getTrianglePath() const {
  auto triBound = QRectF(centerPoint - QPointF(circleRadius / 2, lineLength),
                         centerPoint + QPointF(circleRadius / 2, 0));
  auto painterPath = QPainterPath();

  auto bf = rotatePoint(centerPoint, triBound.bottomLeft(), rotation);
  auto t = rotatePoint(
      centerPoint, QPointF(triBound.center().x(), triBound.top()), rotation);
  auto br = rotatePoint(centerPoint, triBound.bottomRight(), rotation);

  painterPath.moveTo(bf);
  painterPath.lineTo(t);
  painterPath.lineTo(br);
  painterPath.closeSubpath();
  return painterPath;
}

bool RotationSelectController::ifHitTrianglePath(const QPointF& p) const {
  return getTrianglePath().contains(p);
}

bool RotationSelectController::ifHitCenterPoint(const QPointF& p) const {
  auto line = QLineF(centerPoint, p);
  return line.length() < this->circleRadius;
}

void RotationSelectController::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
  // set cursor
  static auto rotationImage = QPixmap(":/icon/rotation.png")
                                  .scaledToHeight(20, Qt::SmoothTransformation);

  if (ifHitTrianglePath(event->scenePos())) {
    QCursor cursor(rotationImage);
    setCursor(cursor);
    return;
  }
  if (ifHitCenterPoint(event->scenePos())) {
    setCursor(Qt::SizeAllCursor);
    return;
  }

  setCursor(Qt::ArrowCursor);
}
//  2  |  1 -> p.y < 0
// ----------
//   3 |  4 -> p.y > 0
double getAngleFromPoint(const QPointF& p) {
  double x = p.x();
  double y = -p.y();

  if (y == 0) {
    return x >= 0 ? 90 : 270;
  }

  auto tan = qAtan(x / y);

  if (tan >= 0) {
    return x >= 0 ? tan : M_PI + tan;
  }
  return x >= 0 ? M_PI + tan : 2 * M_PI + tan;
}



void RotationSelectController::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  AbstractSelectController::mouseMoveEvent(event);
  if (dragState == ROTATION) {
    auto rotationP = event->scenePos() - boundRect.center();
    double oldAngle = rotation;
    auto nowAngle = getAngleFromPoint(rotationP);
    this->rotation = nowAngle;
    // call the func with delta
    controllerRotating(nowAngle - oldAngle);
    // fix the center point because it may move when call set bound
    centerPoint = startDragPoint;
    update();
  }
  if (dragState == CENTER) {
    if (ifAutoMoveUpdate) {
      centerPoint = event->scenePos();
    }
    controllerCenterDrag(event->scenePos());
    update();
  }
}

void RotationSelectController::mousePressEvent(
    QGraphicsSceneMouseEvent* event) {

    // accept when hit the controller point
  if (ifHitTrianglePath(event->scenePos())) {
    event->accept();
    startDragRotation = this->rotation;
    dragState = ROTATION;
    startDragPoint = this->centerPoint;
    controllerStartDrag(event->scenePos());
    return;
  }

  if (ifHitCenterPoint(event->scenePos())) {
    event->accept();
    dragState = CENTER;
    startDragPoint = this->centerPoint;
    controllerStartDrag(event->scenePos());
    return;
  }
  dragState = NONE;
  event->ignore();
}

void RotationSelectController::mouseReleaseEvent(
    QGraphicsSceneMouseEvent* event) {
  controllerEndDrag(event->scenePos());
  dragState = NONE;
}

RotationSelectController::RotationSelectController(QGraphicsItem* parent)
    : AbstractSelectController(parent) {
  setAcceptHoverEvents(true);
}

QPointF RotationSelectController::getCenterPoint() const {
  return this->centerPoint;
}

void RotationSelectController::setRadius(float radius) {
  this->circleRadius = radius;
}

void RotationSelectController::setLineLength(float length) {
  this->lineLength = length;
}

void RotationSelectController::paint(QPainter* painter,
                                     const QStyleOptionGraphicsItem* option,
                                     QWidget* widget) {
  if (boundRect == QRect()) {
    return;
  }
  auto brush = QBrush(Qt::red);
  auto pen = QPen(Qt::transparent);

  painter->setBrush(brush);
  painter->setPen(pen);
  painter->drawEllipse(centerPoint, circleRadius, circleRadius);

  const auto& painterPath = getTrianglePath();
  painter->drawPath(painterPath);
}

QRectF RotationSelectController::boundingRect() const {
  auto topLeft = centerPoint - QPointF(lineLength, lineLength);
  auto bottomRight = centerPoint + QPointF(lineLength, lineLength);
  return {topLeft, bottomRight};
}

void RotationSelectController::setBoundRect(const QRectF& rect) {
  AbstractSelectController::setBoundRect(rect);
  this->centerPoint = rect.center();
}

// rotate point base on base
// delta is radian
QPointF RotationSelectController::rotatePoint(const QPointF& base,
    const QPointF& target, double delta) {
  double cosAngle = qCos(delta);
  double sinAngle = qSin(delta);

  double translatedX = target.x() - base.x();
  double translatedY = target.y() - base.y();

  double rotatedX = translatedX * cosAngle - translatedY * sinAngle;
  double rotatedY = translatedX * sinAngle + translatedY * cosAngle;

  double finalX = rotatedX + base.x();
  double finalY = rotatedY + base.y();

  return {finalX, finalY};
}
}  // namespace Scene