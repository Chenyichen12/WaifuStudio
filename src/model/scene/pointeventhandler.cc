#include "pointeventhandler.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "scenecontroller.h"
namespace Scene {
bool PointEventHandler::contain(float x1, float y1, float x2, float y2,
                                double scale) {
  // normally one view in a project
  float length = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
  auto testRadius = AbsolutePointRadius / scale;
  return length < testRadius * testRadius;
}

std::optional<int> PointEventHandler::gabPointAt(float x, float y,
                                                 double scale) {
  const auto& vec = controller->getPointFromScene();
  for (int i = 0; i < vec.size(); ++i) {
    const auto& testPoint = vec[i];
    if (contain(x, y, testPoint.x(), testPoint.y(), scale)) {
      return i;
    }
  }
  return std::nullopt;
}

void PointEventHandler::pointPressedEvent(int index,
                                          QGraphicsSceneMouseEvent* event) {}

void PointEventHandler::pointMoveEvent(int current,
                                       QGraphicsSceneMouseEvent* event) {}

void PointEventHandler::pointReleaseEvent(QGraphicsSceneMouseEvent* event) {}

PointEventHandler::PointEventHandler(AbstractController* controller) {
  this->controller = controller;
}

double PointEventHandler::getScaleFromTheView(const QGraphicsScene* whichScene,
    const QWidget* whichWidget) {
  for (const auto& view : whichScene->views()) {
    if (view->viewport() == whichWidget) {
      return view->transform().m11();
    }
  }
  return 1;
}

void PointEventHandler::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  const auto& pos = event->scenePos();
  double scale = getScaleFromTheView(controller->scene(), event->widget());
  auto index = gabPointAt(pos.x(), pos.y(), scale);
  if (index.has_value()) {
    this->pointPressedEvent(index.value(), event);
    currentIndex = index.value();
    event->accept();
  } else {
    this->pointPressedEvent(-1, event);
    currentIndex = -1;
    event->ignore();
  }
}

void PointEventHandler::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  this->pointMoveEvent(currentIndex, event);
}

void PointEventHandler::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  this->pointReleaseEvent(event);
  this->currentIndex = -1;
}
}  // namespace Scene
