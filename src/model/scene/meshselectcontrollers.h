#pragma once

#include "../command/controllercommand.h"
#include "selectcontrollers.h"
namespace Scene {
class AbstractController;
/**
 * handle the rect select of the mesh controller
 * it should change the position of the mesh point
 */
class AbstractRectSelectController : public RectSelectController {
 private:
  // need info of the start rect
  QRectF startDragRect;

 protected:
  AbstractController* controller;
  std::vector<Command::ControllerCommandInfo> startPointPos;
  void rectMoving(const QPointF& pre, const QPointF& aft) override;
  /**
   * handle to move point of the select point
   * @param pos
   */
  void rectStartMove(const QPointF& pos) override;
  /**
   * will clear the startPointPos in end move
   * @param startPoint
   * @param endPoint
   */
  void rectEndMove(const QPointF& startPoint, const QPointF& endPoint) override;
  /**
   * drag handler
   * the mesh is make up with 9 controller point
   *
   * @param aft drag mouse position
   */
  void leftTopDrag(const QPointF& aft);
  void leftBottomDrag(const QPointF& aft);
  void rightTopDrag(const QPointF& aft);
  void rightBottomDrag(const QPointF& aft);
  void handleMouseAt(const QRectF& aftRect, bool isXFlip, bool isYFlip);

  virtual std::vector<int> getSelectIndex() = 0;

  /**
   * call it to announce move event;
   */
  virtual void pointsHaveMoved() {}

 public:
  AbstractRectSelectController(AbstractController* controller);
};

class AbstractRotationSelectController : public RotationSelectController {
 private:
  AbstractController* controller;
 protected:

 std::vector<Command::ControllerCommandInfo> startPointPos;

  void controllerCenterDrag(const QPointF& mouseScenePoint) override;
  void controllerRotating(double rotateDelta) override;
  void controllerStartDrag(const QPointF& mouseScenePos) override;
  void controllerEndDrag(const QPointF& mouseScenePos) override;

  /**
   * the select index of the controller
   * return all index to affect all point
   */
  virtual std::vector<int> getSelectIndex() = 0;

  /**
   * call function when point moved
   */
  virtual void pointsHaveMoved() {}
 public:
  AbstractRotationSelectController(AbstractController* controller);
};
}  // namespace Scene
