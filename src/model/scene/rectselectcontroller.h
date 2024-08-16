#pragma once
#include <QGraphicsItem>
/**
 * one special controller not has controller id, just a helper controller to
 * help control the point
 */

namespace Scene {
class RectSelectController : public QGraphicsItem {
 private:
  QRectF boundRect;

  double padding = 10;
  double lineWidth = 5;

  // the start point and end point of once movement
  QPointF startPoint;
  QPointF endPoint;

  QPointF lastMovePoint;
  /**
   * test the rect hit, default the rect will be hit at border
   * @param p scene hit point
   * @return
   */
  virtual bool ifHitRectBorder(const QPointF& p) const;

 public:
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  /**
   * get the bound from the list of point from scene
   * @param list scene point list
   * @return bound rect, not contain padding
   */
  static QRectF boundRectFromPoints(const std::vector<QPointF>& list);

  /**
   * the appearance information of rect
   * also affect the event hit test
   * position from scene
   */
  void setBoundRect(const QRectF& rect);
  void setPadding(double padding);
  void setLineWidth(double lineWidth);

  RectSelectController(QGraphicsItem* parent = nullptr);

  /**
   * call the function when the rect is moving
   * get the delta by  p2 - p1
   * point position is in scene position
   */
  std::function<void(const QPointF&, const QPointF&)> moveCallBack =
      [](const auto& a, const auto& b) {};
  /**
   * call the function when the rect move end
   * the first point pargma is the start position
   * the second point pargma is the end position
   */
  std::function<void(const QPointF&, const QPointF&)> moveEndCallBack =
      [](const QPointF&, const QPointF&) {};
  /**
   * normally the rect will auto update position in move event
   * set it false to update manually update the rect select position
   */
  bool ifAutoMoveUpdate = true;

 protected:
  /**
   * when the mouse hit the item rect select controller will grab the event
   */
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  virtual void rectMoving(const QPointF& pre, const QPointF& aft) {}
  virtual void rectStartMove(const QPointF& pos) {}
  virtual void rectEndMove(const QPointF& startPoint, const QPointF& endPoint) {}

};
}  // namespace Scene
