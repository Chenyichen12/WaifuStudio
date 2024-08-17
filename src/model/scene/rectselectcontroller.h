#pragma once
#include <QGraphicsItem>
/**
 * one special controller not has controller id, just a helper controller to
 * help control the point
 */

namespace Scene {

class AbstractSelectController: public QGraphicsItem {
public:
  AbstractSelectController(QGraphicsItem* parent = nullptr);

  /**
   * normally the select controller will have a bound of selected point
   * it will be set from actual controller
   * @param pointList 
   */
  virtual void setBoundRect(const std::vector<QPointF>& pointList);
  virtual void setBoundRect(const QRectF& rect) { this->boundRect = rect;}
  /**
   * get the bound from the list of point from scene
   * @param list scene point list
   * @return bound rect, not contain padding
   */
  static QRectF boundRectFromPoints(const std::vector<QPointF>& list);


 protected:
  /**
   * the bound is different from boundingRect()
   * the bound is the minutest bound of the select point which is defined by
   * controller
   */
  QRectF boundRect;
};


class RectSelectController : public AbstractSelectController {
 protected:
  /**
   * the state when in drag
   * normally one controller has 10 control point
   * which is 9 point in border and one point in center
   */
  enum HandledState {

    LEFT_TOP = 0,
    TOP,
    RIGHT_TOP,
    LEFT,
    CENTER,
    RIGHT,
    LEFT_BUTTON,
    BUTTON,
    RIGHT_BUTTON,
    NONE

  };
  /**
   * test the rect hit
   * @param p scene hit point
   * @return
   */
  HandledState ifHitControllerPoint(const QPointF& p) const;
  /**
   * @param state which control point
   * @return the actual visible rect of the control point
   */
  QRectF getHandlerVisibleRect(HandledState state) const;
  /**
   * 
   * @param state 
   * @return the actual scene point of the control point
   */
  QPointF getHandlerHitPoint(HandledState state) const;

 private:
  double padding = 10;
  double lineWidth = 5;



  QPointF lastMovePoint;

 public:
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  

  /**
   * the appearance information of rect
   * also affect the event hit test
   * position from scene
   */
  void setBoundRect(const QRectF& rect) override;
  void setPadding(double padding);
  void setLineWidth(double lineWidth);

  RectSelectController(QGraphicsItem* parent = nullptr);

  /**
   * normally the rect will auto update position in move event
   * set it false to update manually update the rect select position
   */
  bool ifAutoMoveUpdate = true;

 protected:
 
  /**
   * in one drag, normally the user will drag one of the drag point
   * change it in press event
   */
  HandledState dragState;
  // the start point and end point of once movement
  QPointF startPoint;
  QPointF endPoint;
  /**
   * when the mouse hit the item rect select controller will grab the event
   */
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  /**
   * call the function when the rect is moving
   * get the delta by  p2 - p1
   * point position is in scene position
   */
  virtual void rectMoving(const QPointF& pre, const QPointF& aft) {}
  virtual void rectStartMove(const QPointF& pos) {}
  /**
   * call the function when the rect move end
   * the first point pargma is the start position
   * the second point pargma is the end position
   */
  virtual void rectEndMove(const QPointF& startPoint, const QPointF& endPoint) {}
};
}  // namespace Scene
