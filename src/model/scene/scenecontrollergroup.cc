#include "scenecontrollergroup.h"

#include "scenecontroller.h"
namespace Scene {
void SceneControllerGroup::pushBackController(AbstractController* controller) {
  this->controllers.push_back(controller);
  this->addToGroup(controller);
  controller->setZValue(controllers.size());
}
void SceneControllerGroup::pushFrontController(AbstractController* controller) {
  this->controllers.push_front(controller);
  this->addToGroup(controller);
  for (int i = 0; i < controllers.size(); ++i) {
    controllers[i]->setZValue(i);
  }
}
}  // namespace Scene