#pragma once
#include <optional>
class QGraphicsSceneMouseEvent;
class QGraphicsScene;
class QWidget;
namespace Scene {
class AbstractController;

/**
 * this is a helper class
 * it need one controller which has some controller point
 * then it will help the controller to handle mousePressed, mouseMove,
 * mouseRelease event if the press is valid it will call point pressed event
 * point move event and release event rewrite it to handler change controller
 */
class PointEventHandler {
 private:
  AbstractController* controller;
  /**
   * check if the testPoint circle is close enough to another point
   * @param x1 firstPoint
   * @param y1
   * @param x2 secondPoint
   * @param y2
   * @param scale which is the scale of the event view
   * used to define the testPoint is close enough
   * @return
   */
  static bool contain(float x1, float y1, float x2, float y2, double scale);
  /**
   * find the controller mesh point index contain the testPoint
   * @param x testPoint
   * @param y
   * @param scale testLength
   * @return if not found return nullopt
   */
  std::optional<int> gabPointAt(float x, float y, double scale);

 protected:
  int currentIndex = -1;

  /**
   * @param index if event doesn't hit point the index == -1
   * @param event 
   */
  virtual void pointPressedEvent(int index, QGraphicsSceneMouseEvent* event);
  virtual void pointMoveEvent(int current, QGraphicsSceneMouseEvent* event);
  virtual void pointReleaseEvent(QGraphicsSceneMouseEvent* event);

 public:
  static constexpr float AbsolutePointRadius = 3;
  virtual ~PointEventHandler() = default;
  PointEventHandler(AbstractController* controller);

  static double getScaleFromTheView(const QGraphicsScene* whichScene,
                                    const QWidget* whichWidget);

  /**
   * accept the event when the event hit one point
   * @param event 
   */
  void mousePressEvent(QGraphicsSceneMouseEvent* event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
};
}  // namespace Scene
