//
// Created by chenyichen on 2024/9/19.
//

#pragma once
#include <QGraphicsRectItem>

namespace WaifuL2d {
class OperatePoint : public QGraphicsRectItem {
  bool isPressed = false;
  int radius = 4;
  typedef void PointSelectedChange(bool isSelected, const QVariant& data);
  typedef void PointShouldMove(const QPointF& point, bool isStart,
                               const QVariant& data);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public:
  explicit OperatePoint(QGraphicsItem* parent = nullptr);
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = nullptr) override;
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                      const QVariant& value) override;
  void setRadius(int r);
  int getRadius() const { return radius; }

  bool isHitPoint(const QPointF& point) const;
  bool isHitPoint(const QPointF& point, const QWidget* whereEvent) const;

  QVariant data;

  std::function<PointSelectedChange> pointSelectedChange = nullptr;
  std::function<PointShouldMove> pointShouldMove = nullptr;
};

class OperateRectangle : public QGraphicsItem {
  std::array<OperatePoint*, 9> operatePoints;
  std::array<OperatePoint*, 4> operateRotationPoints;
  QRectF rect;
  static constexpr int padding = 5;
  QRectF getTransformRectF() const;

  typedef void RectShouldResize(const QRectF& startRect,
                                const QRectF& newRect,
                                bool xFlip,
                                bool yFlip,
                                bool isStart,
                                const QVariant& data);
  typedef void RectShouldRotate(const QPointF& rotationCenter, qreal angle,
                                bool isStart,
                                const QVariant& data);

  struct {
    QRectF startRect;
    bool isFitRadio = false;
  } startRectRecord;

  struct {
    QLineF startLine;
  } startRotateRecord;

  void handleRotateMove(int which, const QPointF& where, bool isStart);
  void handleRectPointMove(int which, const QPointF& where, bool isStart);
  qreal getViewPortScale() const;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

public:
  OperateRectangle(QGraphicsItem* parent = nullptr);
  void setRect(const QRectF& rect);

  QRectF getRect() const { return rect; }
  QPainterPath shape() const override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  QVariant data;
  std::function<RectShouldResize> rectShouldResize = nullptr;
  std::function<RectShouldRotate> rectShouldRotate = nullptr;
};
} // namespace WaifuL2d