//
// Created by chenyichen on 2024/9/19.
//
#include "operatepoint.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>

namespace WaifuL2d {
OperatePoint::OperatePoint(QGraphicsItem* parent) : QGraphicsRectItem(parent) {
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
  this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
  this->setBrush(QBrush(Qt::white));
  this->setRect(-radius, -radius, 2 * radius, 2 * radius);
}

void OperatePoint::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsRectItem::mousePressEvent(event);
  isPressed = true;
}

void OperatePoint::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  if (pointShouldMove) {
    pointShouldMove(event->scenePos(), isPressed, data);
  }
  isPressed = false;
}

void OperatePoint::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsItem::mouseReleaseEvent(event);
  isPressed = false;
}

void OperatePoint::setRadius(int r) {
  radius = r;
  this->setRect(-r, -r, 2 * r, 2 * r);
}

bool OperatePoint::isHitPoint(const QPointF& point) const {
  qreal scale = 1;
  if (this->scene() && !this->scene()->views().empty()) {
    scale = this->scene()->views().first()->transform().m11();
  }
  auto actualR = radius / scale;
  auto hitLength = this->pos() - point;
  return actualR * actualR >=
         hitLength.x() * hitLength.x() + hitLength.y() * hitLength.y();
}

bool OperatePoint::isHitPoint(const QPointF& p,
                              const QWidget* whereEvent) const {
  qreal scale = 1;
  const auto& views = this->scene()->views();
  auto it = std::find_if(views.begin(), views.end(),
                         [whereEvent](const QGraphicsView* view) {
                           return whereEvent == view->viewport();
                         });
  if (it != views.end()) {
    scale = (*it)->transform().m11();
  }
  auto actualR = radius / scale;
  auto hitLength = this->pos() - p;
  return actualR * actualR >=
         hitLength.x() * hitLength.x() + hitLength.y() * hitLength.y();
}

QVariant OperatePoint::itemChange(QGraphicsItem::GraphicsItemChange change,
                                  const QVariant& value) {
  if (change == QGraphicsItem::ItemSelectedHasChanged) {
    if (this->pointSelectedChange) {
      this->pointSelectedChange(value.toBool(), data);
    }
  }
  return QGraphicsItem::itemChange(change, value);
}

void OperatePoint::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget) {
  auto pen = QPen();
  pen.setWidthF(rect().width() / 10);
  pen.setColor(Qt::black);
  painter->setPen(pen);
  if (this->isSelected()) {
    painter->setBrush(Qt::red);
  } else {
    painter->setBrush(Qt::white);
  }

  painter->drawEllipse(this->rect());
}

class OperateRectPoint : public OperatePoint {
  int role = 4;
  static constexpr std::array<Qt::CursorShape, 9> cursorMap = {
      Qt::SizeFDiagCursor, Qt::SizeVerCursor, Qt::SizeBDiagCursor,
      Qt::SizeHorCursor, Qt::SizeAllCursor, Qt::SizeHorCursor,
      Qt::SizeBDiagCursor, Qt::SizeVerCursor, Qt::SizeFDiagCursor,
  };

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
    OperatePoint::mousePressEvent(event);
    event->accept();
  }

  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override {
    OperatePoint::hoverMoveEvent(event);
    setCursor(QCursor(cursorMap[role]));
  }

public:
  OperateRectPoint(QGraphicsItem* parent = nullptr) : OperatePoint(parent) {
    this->setFlag(ItemIsSelectable, false);
    this->setAcceptHoverEvents(true);
    setRadius(3);
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override {
    painter->setPen(Qt::black);
    painter->setPen(Qt::green);
    painter->setBrush(Qt::green);
    painter->drawRect(this->rect());
  }

  void setRole(const int role) {
    Q_ASSERT(role < 9 && role >= 0);
    this->role = role;
  }
};

class OperateRotatePoint : public OperatePoint {
protected:
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override {
    static auto rotationImage =
        QPixmap(":/icon/rotation.png")
        .scaledToHeight(20, Qt::SmoothTransformation);
    QCursor cursor(rotationImage);
    setCursor(cursor);
  }

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
    OperatePoint::mousePressEvent(event);
    event->accept();
  }

public:
  OperateRotatePoint(QGraphicsItem* parent = nullptr) : OperatePoint(parent) {
    this->setFlag(ItemIsSelectable, false);
    this->setAcceptHoverEvents(true);
    setRadius(3);
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override {
  }
};

QRectF OperateRectangle::getTransformRectF() const {
  qreal scale = getViewPortScale();
  return rect.marginsAdded(
      {padding / scale, padding / scale, padding / scale, padding / scale});
}

void OperateRectangle::handleRotateMove(int which, const QPointF& where,
                                        bool isStart) {
  if (isStart) {
    auto origin = this->operateRotationPoints[which]->pos();
    origin = mapToScene(origin);
    auto center = mapToScene(rect.center());

    this->startRotateRecord.startLine = QLineF(center, origin);
  }

  if (!this->rectShouldRotate) {
    return;
  }

  auto startCenter = this->startRotateRecord.startLine.p1();
  auto newLine = QLineF(startCenter, where);
  qreal angle = newLine.angleTo(this->startRotateRecord.startLine);
  angle = angle * 2 * M_PI / 360;
  this->rectShouldRotate(startCenter, angle, isStart, this->data);
}

void OperateRectangle::handleRectPointMove(int which, const QPointF& where,
                                           bool isStart) {
  if (isStart) {
    startRectRecord.isFitRadio = false;
    startRectRecord.startRect = rect;
    startRectRecord.startRect.moveTopLeft(this->pos());
  }

  if (!rectShouldResize) {
    return;
  }

  auto startRect = startRectRecord.startRect;

  bool isXFlip;
  bool isYFlip;
  QRectF newRect;

  // if width == 0 or height == 0 it will crash
  if (startRect.width() == 0 || startRect.height() == 0) {
    return;
  }

  if (which == 4) {
    newRect = startRect;
    newRect.moveCenter(where);
    rectShouldResize(startRect, newRect, false, false, isStart, data);
    return;
  }

  auto targetPoint = where;

  auto isFourEdgePoint = (which == 0 || which == 2 || which == 6 || which == 8);
  if (startRectRecord.isFitRadio && isFourEdgePoint) {
    auto radio = startRect.width() / startRect.height();
    auto relativePoint = QPointF();
    switch (which) {
      case 0:
        relativePoint = startRect.bottomRight();
        break;
      case 2:
        relativePoint = startRect.bottomLeft();
        break;
      case 6:
        relativePoint = startRect.topRight();
        break;
      case 8:
        relativePoint = startRect.topLeft();
        break;
      default:
        Q_ASSERT(false);
        break;
    }

    auto width = targetPoint.x() - relativePoint.x();
    if (which == 2 || which == 6) {
      width = -width;
    }
    auto newHeight = width / radio;
    targetPoint.setY(relativePoint.y() + newHeight);
  }

  switch (which) {
    case 0:
    case 3:
    case 6: {
      if (targetPoint.x() > startRect.right()) {
        isXFlip = true;
        newRect.setX(startRect.right());
      } else {
        isXFlip = false;
        newRect.setX(targetPoint.x());
      }

      newRect.setWidth(qAbs(targetPoint.x() - startRect.right()));
      break;
    }
    case 2:
    case 5:
    case 8: {
      if (targetPoint.x() < startRect.left()) {
        isXFlip = true;
        newRect.setX(targetPoint.x());
      } else {
        isXFlip = false;
        newRect.setX(startRect.left());
      }

      newRect.setWidth(qAbs(targetPoint.x() - startRect.left()));
      break;
    }
    default: {
      isXFlip = false;
    }
  }

  switch (which) {
    case 0:
    case 1:
    case 2: {
      if (targetPoint.y() > startRect.bottom()) {
        isYFlip = true;
        newRect.setY(startRect.bottom());
      } else {
        isYFlip = false;
        newRect.setY(targetPoint.y());
      }

      newRect.setHeight(qAbs(targetPoint.y() - startRect.bottom()));
      break;
    }
    case 6:
    case 7:
    case 8: {
      if (targetPoint.y() < startRect.top()) {
        isYFlip = true;
        newRect.setY(targetPoint.y());
      } else {
        isYFlip = false;
        newRect.setY(startRect.top());
      }
      newRect.setHeight(qAbs(targetPoint.y() - startRect.top()));
      break;
    }
    default: {
      isYFlip = false;
    }
  }
  switch (which) {
    case 1:
    case 7: {
      newRect.setX(startRect.x());
      newRect.setWidth(startRect.width());
      break;
    }
    case 3:
    case 5: {
      newRect.setY(startRect.y());
      newRect.setHeight(startRect.height());
      break;
    }
    default:
      break;
  }

  rectShouldResize(startRect, newRect, isXFlip, isYFlip, isStart, data);
}

qreal OperateRectangle::getViewPortScale() const {
  double scale = 1;
  if (this->scene() && !this->scene()->views().empty()) {
    scale = this->scene()->views().first()->transform().m11();
  }
  return scale;
}

void OperateRectangle::keyPressEvent(QKeyEvent* event) {
  QGraphicsItem::keyPressEvent(event);
  startRectRecord.isFitRadio = true;
}

void OperateRectangle::keyReleaseEvent(QKeyEvent* event) {
  QGraphicsItem::keyReleaseEvent(event);
  startRectRecord.isFitRadio = false;
}

OperateRectangle::OperateRectangle(QGraphicsItem* parent)
  : QGraphicsItem(parent) {
  setFlag(ItemIsFocusable, true);
  for (int i = 0; i < 9; i++) {
    auto point = new OperateRectPoint(this);
    point->data = i;
    point->pointShouldMove = [this](const QPointF& point, bool isStart,
                                    const QVariant& pointData) {
      if (isStart) {
        this->grabKeyboard();
      }
      this->handleRectPointMove(pointData.toInt(), point, isStart);
    };
    point->setRole(i);
    operatePoints[i] = point;
  }

  for (int i = 0; i < 4; i++) {
    auto point = new OperateRotatePoint(this);
    point->data = i;
    point->pointShouldMove = [this](const QPointF& point, bool isStart,
                                    const QVariant& pointData) {
      this->handleRotateMove(pointData.toInt(), point, isStart);
    };

    operateRotationPoints[i] = point;
  }
}

void OperateRectangle::setRect(const QRectF& rect) {
  this->rect = {0, 0, rect.width(), rect.height()};
  this->setPos(rect.topLeft());
  update();
}

QPainterPath OperateRectangle::shape() const {
  auto r = getTransformRectF();
  auto path = QPainterPath();
  path.addRect(r);
  return path;
}

QRectF OperateRectangle::boundingRect() const { return rect; }

void OperateRectangle::paint(QPainter* painter,
                             const QStyleOptionGraphicsItem* option,
                             QWidget* widget) {
  auto r = getTransformRectF();
  auto points = r.topLeft();
  auto width = r.width();
  auto height = r.height();
  operatePoints[0]->setPos(points);
  operatePoints[1]->setPos(points + QPointF(width / 2, 0));
  operatePoints[2]->setPos(points + QPointF(width, 0));
  operatePoints[3]->setPos(points + QPointF(0, height / 2));
  operatePoints[4]->setPos(points + QPointF(width / 2, height / 2));
  operatePoints[5]->setPos(points + QPointF(width, height / 2));
  operatePoints[6]->setPos(points + QPointF(0, height));
  operatePoints[7]->setPos(points + QPointF(width / 2, height));
  operatePoints[8]->setPos(points + QPointF(width, height));

  auto scale = getViewPortScale();
  auto margin = 10 / scale;
  auto rOuter = r.marginsAdded({margin, margin, margin, margin});
  operateRotationPoints[0]->setPos(rOuter.topLeft());
  operateRotationPoints[1]->setPos(rOuter.topRight());
  operateRotationPoints[2]->setPos(rOuter.bottomLeft());
  operateRotationPoints[3]->setPos(rOuter.bottomRight());

  auto pen = QPen();
  pen.setWidthF(1 / painter->transform().m11());
  pen.setColor(Qt::green);
  painter->setPen(pen);
  painter->drawRect(r);
}
} // namespace WaifuL2d