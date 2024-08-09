#pragma once
#include <QGraphicsItemGroup>
namespace Scene {
class AbstractController;
class SceneControllerGroup : public QGraphicsItemGroup {
 private:
  QList<AbstractController*> controllers;

 public:
  void pushBackController(AbstractController* controller);
  void pushFrontController(AbstractController* controller);
};

}  // namespace Scene