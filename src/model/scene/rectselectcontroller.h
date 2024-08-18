#pragma once
#include <QGraphicsItem>
/**
 * select controller is a controller to control the select point
 * normally, it just a temp controller. will disappear when the selection is null
 * the actual select controller should record the motion and push undo command
 * to the stack in root controller
 */

namespace Scene {

/**
 * base class for select controller
 */
class AbstractSelectController : public QGraphicsItem {
 public:
  AbstractSelectController(QGraphicsItem* parent = nullptr);

  /**
   * normally the select controller will have a bound of selected point
   * it will be set from actual controller
   * controller should call this function when the selection is changed
   * @param pointList
   */
  virtual void setBoundRect(const std::vector<QPointF>& pointList);
  /**
   * if the rect is QRect() which contains no width or height
   * it will auto call setVisible(false)
   * otherwise, setVisible(true)
   */
  virtual void setBoundRect(const QRectF& rect);
  /**
   * get the bound from the list of point from scene
   * @param list scene point list
   * @return bound rect, not contain padding
   */
  static QRectF boundRectFromPoints(const std::vector<QPointF>& list);

 protected:
  /**
   * the bound is different from boundingRect()
   * normally boundingRect() should return a bigger rect to handle event
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
   * normally the rect will auto update position in move event
   * set it false to update manually update the rect select position
   */
  bool ifAutoMoveUpdate = true;

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
  virtual void rectEndMove(const QPointF& startPoint, const QPointF& endPoint) {
  }
};

class RotationSelectController : public AbstractSelectController {
 private:
  

  float circleRadius = 5;
  float lineLength = 10;

  QPainterPath getTrianglePath() const;
  /**
   *               |
   *              /\
   *              ||
   *              ||   ---> TrianglePath
   *              ||
   *             ****
   *             ****    --> CenterPoint
   *             ****
   */
  bool ifHitTrianglePath(const QPointF& p) const;
  bool ifHitCenterPoint(const QPointF& p) const;

 protected:
  enum HandledState {
    ROTATION = 0, // rotate drag state
    CENTER = 1, // center drag state
    NONE = 2,
  } dragState;


  QPointF centerPoint;// center point
  bool ifAutoMoveUpdate = true;
  double rotation = 0; // current rotation of the controller
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  virtual void controllerRotating(double rotateDelta) {}
  virtual void controllerCenterDrag(const QPointF& mouseScenePoint) {}
  virtual void controllerStartDrag(const QPointF& mouseScenePos) {}
  virtual void controllerEndDrag(const QPointF& mouseScenePos) {}

  double startDragRotation = 0; // current rotation when the drag starts
  QPointF startDragPoint; // current point when the drag starts

 public:
  RotationSelectController(QGraphicsItem* parent = nullptr);
  QPointF getCenterPoint() const;
  void setRadius(float radius); // set center point radius of the controller 
  void setLineLength(float length); // set length of the controller line

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  QRectF boundingRect() const override;
  void setBoundRect(const QRectF& rect) override;

  static QPointF rotatePoint(const QPointF& base, const QPointF& target,
                             double delta);

};
}  // namespace Scene
