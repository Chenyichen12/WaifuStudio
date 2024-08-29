#include "meshselectcontrollers.h"

#include "scenecontroller.h"

namespace Scene {

void AbstractRectSelectController::rectMoving(const QPointF& pre,
                                              const QPointF& aft) {
  if (dragState == CENTER) {
    auto delta = aft - pre;
    auto pointList = controller->getPointFromScene();
    for (int index : this->getSelectIndex()) {
      auto pos = pointList[index];
      pos += delta;
      controller->setPointFromScene(index, pos);
    }
    this->pointsHaveMoved();
    return;
  }
  // when some point in one line startDragRect will cause error
  // so banned the controller point handler when the height or width == 0
  if (startDragRect.width() != 0 && startDragRect.height() != 0) {
    switch (dragState) {
      case TOP: {
        QPointF p = aft;
        p.setX(startDragRect.left());
        this->leftTopDrag(p);
        break;
      }
      case LEFT: {
        QPointF p = aft;
        p.setY(startDragRect.top());
        this->leftTopDrag(p);
        break;
      }
      case RIGHT: {
        QPointF p = aft;
        p.setY(startDragRect.top());
        this->rightTopDrag(p);
        break;
      }
      case BUTTON: {
        QPointF p = aft;
        p.setX(startDragRect.left());
        this->leftBottomDrag(p);
        break;
      }
      case LEFT_TOP:
        leftTopDrag(aft);
        break;

      case LEFT_BUTTON:
        leftBottomDrag(aft);
        break;
      case RIGHT_TOP:
        rightTopDrag(aft);
        break;
      case RIGHT_BUTTON:
        rightBottomDrag(aft);
        break;
      default:
        break;
    }
    this->pointsHaveMoved();
  }
}

void AbstractRectSelectController::leftTopDrag(const QPointF& aft) {
  bool isXFlip = aft.x() > startDragRect.right();
  bool isYFlip = aft.y() > startDragRect.bottom();
  QPointF topLeft;
  QPointF bottomRight;
  if (isXFlip) {
    topLeft.setX(startDragRect.right());
    bottomRight.setX(aft.x());
  } else {
    topLeft.setX(aft.x());
    bottomRight.setX(startDragRect.right());
  }

  if (isYFlip) {
    topLeft.setY(startDragRect.bottom());
    bottomRight.setY(aft.y());
  } else {
    topLeft.setY(aft.y());
    bottomRight.setY(startDragRect.bottom());
  }

  handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
}

void AbstractRectSelectController::leftBottomDrag(const QPointF& aft) {
  bool isXFlip = aft.x() > startDragRect.right();
  bool isYFlip = aft.y() < startDragRect.top();
  QPointF topLeft;
  QPointF bottomRight;
  if (isXFlip) {
    topLeft.setX(startDragRect.right());
    bottomRight.setX(aft.x());
  } else {
    topLeft.setX(aft.x());
    bottomRight.setX(startDragRect.right());
  }

  if (isYFlip) {
    topLeft.setY(aft.y());
    bottomRight.setY(startDragRect.top());
  } else {
    topLeft.setY(startDragRect.top());
    bottomRight.setY(aft.y());
  }

  handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
}

void AbstractRectSelectController::rightTopDrag(const QPointF& aft) {
  bool isXFlip = aft.x() < startDragRect.left();
  bool isYFlip = aft.y() > startDragRect.bottom();
  QPointF topLeft;
  QPointF bottomRight;
  if (isXFlip) {
    topLeft.setX(aft.x());
    bottomRight.setX(startDragRect.left());
  } else {
    topLeft.setX(startDragRect.left());
    bottomRight.setX(aft.x());
  }

  if (isYFlip) {
    topLeft.setY(startDragRect.bottom());
    bottomRight.setY(aft.y());
  } else {
    topLeft.setY(aft.y());
    bottomRight.setY(startDragRect.bottom());
  }

  handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
}

void AbstractRectSelectController::rightBottomDrag(const QPointF& aft) {
  bool isXFlip = aft.x() < startDragRect.left();
  bool isYFlip = aft.y() < startDragRect.top();
  QPointF topLeft;
  QPointF bottomRight;
  if (isXFlip) {
    topLeft.setX(aft.x());
    bottomRight.setX(startDragRect.left());
  } else {
    topLeft.setX(startDragRect.left());
    bottomRight.setX(aft.x());
  }

  if (isYFlip) {
    topLeft.setY(aft.y());
    bottomRight.setY(startDragRect.top());
  } else {
    topLeft.setY(startDragRect.top());
    bottomRight.setY(aft.y());
  }

  handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
}

void AbstractRectSelectController::handleMouseAt(const QRectF& aftRect,
                                                 bool isXFlip, bool isYFlip) {
  for (const auto& startPoint : this->startPointPos) {
    float u = (startPoint.p.x() - startDragRect.x()) / startDragRect.width();
    float v = (startPoint.p.y() - startDragRect.y()) / startDragRect.height();

    float x;
    float y;
    if (isXFlip) {
      x = aftRect.right() - aftRect.width() * u;
    } else {
      x = aftRect.width() * u + aftRect.left();
      // float y = aftRect.height() * v + aftRect.y();
    }
    if (isYFlip) {
      y = aftRect.bottom() - aftRect.height() * v;
    } else {
      y = aftRect.height() * v + aftRect.y();
    }
    controller->setPointFromScene(startPoint.index, QPointF(x, y));
  }
}

void AbstractRectSelectController::rectStartMove(const QPointF& pos) {
  auto pointList = controller->getPointFromScene();
  for (int index : this->getSelectIndex()) {
    const auto& p = pointList[index];
    startPointPos.push_back({p, index});
  }
  startDragRect = this->boundRect;
}

void AbstractRectSelectController::rectEndMove(const QPointF& startPoint,
                                               const QPointF& endPoint) {
  RectSelectController::rectEndMove(startPoint, endPoint);
  this->startPointPos.clear();
}

AbstractRectSelectController::AbstractRectSelectController(
    AbstractController* controller)
    : RectSelectController(controller), controller(controller) {
  this->ifAutoMoveUpdate = false;
}
void AbstractRotationSelectController::controllerCenterDrag(
    const QPointF& mouseScenePoint) {
  auto delta = mouseScenePoint - startDragPoint;
  for (const auto& startCommand : startPointPos) {
    auto p = startCommand.p + delta;
    controller->setPointFromScene(startCommand.index, p);
  }
  this->pointsHaveMoved();
}
void AbstractRotationSelectController::controllerRotating(double rotateDelta) {
  const auto& pList = controller->getPointFromScene();
  for (int index : this->getSelectIndex()) {
    auto pos = pList[index];
    auto p = rotatePoint(startDragPoint, pos, rotateDelta);
    controller->setPointFromScene(index, p);
  }
  this->pointsHaveMoved();
}

void AbstractRotationSelectController::controllerStartDrag(
    const QPointF& mouseScenePos) {
  Q_UNUSED(mouseScenePos)
  const auto& pList = controller->getPointFromScene();
  for (int index : this->getSelectIndex()) {
    auto pos = pList[index];
    startPointPos.push_back({pos, index});
  }
}

void AbstractRotationSelectController::controllerEndDrag(
    const QPointF& mouseScenePos) {
  Q_UNUSED(mouseScenePos)
  this->startPointPos.clear();
}

AbstractRotationSelectController::AbstractRotationSelectController(
    AbstractController* controller)
    : RotationSelectController(controller) {
  this->controller = controller;
}
}  // namespace Scene
