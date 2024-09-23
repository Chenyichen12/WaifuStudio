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
  pen.setWidth(rect().width() / 10);
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
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
    OperatePoint::mousePressEvent(event);
    event->accept();
  }

public:
  OperateRectPoint(QGraphicsItem* parent = nullptr) : OperatePoint(parent) {
    this->setFlag(ItemIsSelectable, false);
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override {
    painter->setBrush(Qt::green);
    painter->drawRect(this->rect());
  }
};


QRectF OperateRectangle::getTransformRectF() const {
  double scale = 1;
  if (this->scene() && !this->scene()->views().empty()) {
    scale = this->scene()->views().first()->transform().m11();
  }
  return rect.marginsAdded(
      {padding / scale, padding / scale, padding / scale, padding / scale});
}

void OperateRectangle::handleRectPointMove(int which, const QPointF& where,
                                           bool isStart) {
  if (isStart) {
    startRecord.startRect = rect;
    startRecord.startRect.moveTopLeft(this->pos());
  }

  if (!rectShouldResize) {
    return;
  }

  auto startRect = startRecord.startRect;

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
    rectShouldResize(newRect, false, false, isStart, data);
    return;
  }

  switch (which) {
    case 0:
    case 3:
    case 6: {
      if (where.x() > startRect.right()) {
        isXFlip = true;
        newRect.setX(startRect.right());
      } else {
        isXFlip = false;
        newRect.setX(where.x());
      }

      newRect.setWidth(qAbs(where.x() - startRect.right()));
      break;
    }
    case 2:
    case 5:
    case 8: {
      if (where.x() < startRect.left()) {
        isXFlip = true;
        newRect.setX(where.x());
      } else {
        isXFlip = false;
        newRect.setX(startRect.left());
      }

      newRect.setWidth(qAbs(where.x() - startRect.left()));
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
      if (where.y() > startRect.bottom()) {
        isYFlip = true;
        newRect.setY(startRect.bottom());
      } else {
        isYFlip = false;
        newRect.setY(where.y());
      }

      newRect.setHeight(qAbs(where.y() - startRect.bottom()));
      break;
    }
    case 6:
    case 7:
    case 8: {
      if (where.y() < startRect.top()) {
        isYFlip = true;
        newRect.setY(where.y());
      } else {
        isYFlip = false;
        newRect.setY(startRect.top());
      }
      newRect.setHeight(qAbs(where.y() - startRect.top()));
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

  rectShouldResize(newRect, isXFlip, isYFlip,
                   isStart, data);
}

OperateRectangle::OperateRectangle(QGraphicsItem* parent)
  : QGraphicsItem(parent) {
  for (int i = 0; i < 9; i++) {
    auto point = new OperateRectPoint(this);
    point->data = i;
    point->pointShouldMove = [this](const QPointF& point, bool isStart,
                                    const QVariant& data) {
      this->handleRectPointMove(data.toInt(), point, isStart);
    };
    operatePoints[i] = point;
  }
}

OperateRectangle::~OperateRectangle() {
  for (auto& op : operatePoints) {
    delete op;
  }
}

void OperateRectangle::setRect(const QRectF& rect) {
  this->rect = {0, 0, rect.width(), rect.height()};
  this->setPos(rect.topLeft());
  qDebug() << rect;
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
}
} // namespace WaifuL2d