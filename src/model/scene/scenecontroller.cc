#include "scenecontroller.h"

#include <qevent.h>
#include <QGraphicsScene>
namespace Scene {
void AbstractController::setControllerParent(AbstractController* controller) {
  if (controllerParent != nullptr) {
    auto& child = controllerParent->controllerChildren;
    auto findIter = std::find(child.begin(), child.end(), this);
    if (findIter != child.end()) {
      child.erase(findIter);
    }
  }
  this->controllerParent = controller;
  controller->controllerChildren.push_back(this);
}

QRectF RootController::boundingRect() const {
  return QRectF(0, 0, width, height);
}

RootController::RootController(int width, int height) {
  this->width = width;
  this->height = height;
  this->setVisible(true);
}

void RootController::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {}

int RootController::controllerId() { return -1; }

int RootController::type() const { return ControllerType::RootControllerType; }

void controllerRur(AbstractController* parent,const std::function<RootController::forEachCallBack>& callback) {
  for (auto& controller : parent->getControllerChildren()) {
    if (!callback(controller)) {
      return;
    }
    controllerRur(controller, callback);
  }
}

void RootController::
forEachController(const std::function<forEachCallBack>& callback) {
  controllerRur(this, callback);
}

QPointF RootController::localPointToScene(const QPointF& point) {
  auto x = point.x() * this->width;
  auto y = point.y() * this->height;
  return {x, y};
}

QPointF RootController::scenePointToLocal(const QPointF& point) {
  auto x = point.x() / this->width;
  auto y = point.y() / this->height;
  return {x, y};
}
}  // namespace Scene
