#include "scenecontroller.h"

#include <QUndoStack>

#include "editmeshcontroller.h"

namespace Scene {
std::vector<QPointF> AbstractController::getPointFromLocal() {
  const auto& p = this->getPointFromScene();
  auto res = std::vector<QPointF>();
  for (const auto& point : p) {
    res.emplace_back(this->scenePointToLocal(point));
  }
  return res;
}

void AbstractController::setControllerSelectAble(bool isSelected) {
  this->selectAble = isSelected;
  if (!isSelected) {
    this->setVisible(false);
  }
}

void AbstractController::selectTheController() {
  if (this->selectAble) {
    this->setVisible(true);
  }
}
void AbstractController::setControllerParent(AbstractController* controller) {
  if (controllerParent != nullptr) {
    auto& child = controllerParent->controllerChildren;
    auto findIter = std::find(child.begin(), child.end(), this);
    if (findIter != child.end()) {
      child.erase(findIter);
    }
  }
  this->controllerParent = controller;
  if (controllerParent != nullptr) {
    controller->controllerChildren.push_back(this);
  }
}

RootController::RootController(int width, int height) {
  this->width = width;
  this->height = height;
  this->setVisible(true);
}

void RootController::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {}
QRectF RootController::boundingRect() const {
  return QRectF(0, 0, width, height);
}

int RootController::controllerId() { return -1; }

int RootController::type() const { return ControllerType::RootControllerType; }

void controllerRur(
    const AbstractController* parent,
    const std::function<RootController::forEachCallBack>& callback) {
  for (auto& controller : parent->getControllerChildren()) {
    if (!callback(controller)) {
      return;
    }
    controllerRur(controller, callback);
  }
}

void RootController::forEachController(
    const std::function<forEachCallBack>& callback) const {
  controllerRur(this, callback);
}

std::vector<AbstractController*> RootController::getSelectedChildren() const {
  auto res = std::vector<AbstractController*>();
  this->forEachController([&res](auto* controller) {
    if (controller->isControllerSelected()) {
      res.emplace_back(controller);
    }
    return true;
  });
  return res;
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

void RootController::setUndoStack(QUndoStack* stack) {
  this->controllerUndoStack = stack;
}

void RootController::pushUndoCommand(QUndoCommand* command) {
  if (controllerUndoStack != nullptr) {
    this->controllerUndoStack->push(command);
  }
}

void RootController::setActiveSelectController(ActiveSelectController controller) {
  this->activeSelectController = controller;
  this->forEachController([&](AbstractController* c) {
    c->setActiveSelectController(controller);
    return true;
  });
}

void RootController::setEditMeshController(AbstractController* controller) {
  // delete the pre controller if so
  if (this->editController != nullptr) {
    removeEditMeshController();
  }
  controller->setControllerParent(this);
  controller->setActiveSelectController(this->activeSelectController);
  forEachController([&](auto c) {
    if (c->type() != EditMeshControllerType) {
      c->unSelectTheController();
    }
    return true;
  });
  this->editController = controller;
}


void RootController::removeEditMeshController() {
  if (editController != nullptr) {
    editController->setControllerParent(nullptr);
    delete editController;
    editController = nullptr; 
  }
}

RootController* RootController::findRootController(
    const AbstractController* controller) {
  auto parent = controller->getControllerParent();
  while (parent != nullptr) {
    if (parent->type() == ControllerType::RootControllerType) {
      return static_cast<RootController*>(parent);
    }
    parent = parent->getControllerParent();
  }
  return nullptr;
}
}  // namespace Scene
