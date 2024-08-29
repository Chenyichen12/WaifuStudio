#pragma once

#include "selectcontrollers.h"
#include "../command/controllercommand.h"
namespace Scene {
class AbstractController;
/**
 * handle the rect select of the mesh controller
 * it should change the position of the mesh point
 */
class MeshRectSelectController : public RectSelectController {
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
  virtual void pointsHaveMoved(){}
 public:
  MeshRectSelectController(AbstractController* controller);

};
}  // namespace Scene
