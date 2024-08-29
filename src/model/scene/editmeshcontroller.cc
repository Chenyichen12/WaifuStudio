#include "editmeshcontroller.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "../command/controllercommand.h"
#include "pointeventhandler.h"
#include "scenecontroller.h"
namespace Scene {

class EditMeshPointUndoCommand : public Command::ControllerCommand {
  bool first = true;
 public:
  EditMeshPointUndoCommand(EditMeshController* controller, QUndoCommand* parent = nullptr)
      : ControllerCommand(controller, parent) {}

  void redo() override {
    if (first) {
      first = false;
      return;
    }

    ControllerCommand::redo();
  }
};

class EditMeshPointHandler : public PointEventHandler {
 private:
  EditMeshController* controller;
  QPointF startPoint;
 protected:
  void pointMoveEvent(int current, QGraphicsSceneMouseEvent* event) override {
    if (current == -1) {
      return;
    }
    controller->setPointFromScene(current, event->scenePos());
  }
  void pointPressedEvent(int index, QGraphicsSceneMouseEvent* event) override {
    if (index == -1) {
      controller->unSelectPoint();
    } else {
      if (event->modifiers() != Qt::ShiftModifier) {
        controller->unSelectPoint();
      }
      controller->selectPoint(index);
      startPoint = controller->getPointFromScene()[index];
      event->accept();
    }
  }
  
  void pointReleaseEvent(QGraphicsSceneMouseEvent* event) override {
    auto command = new EditMeshPointUndoCommand(controller);
    command->addOldInfo({startPoint, currentIndex});
    command->addNewInfo({event->scenePos(), currentIndex});
    controller->addUndoCommand(command);
  }

 public:
  explicit EditMeshPointHandler(EditMeshController* controller)
      : PointEventHandler(controller) {
    this->controller = controller;
  }
};


void EditMeshController::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  this->pointHandler->mouseMoveEvent(event);
}

void EditMeshController::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  pointHandler->mousePressEvent(event);
}

void EditMeshController::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  pointHandler->mouseReleaseEvent(event);
}

EditMeshController::EditMeshController(
    const std::vector<MeshVertex>& vertices,
    const std::vector<unsigned int>& incident, QGraphicsItem* parent)
    : AbstractController(parent), vertices(vertices), incident(incident) {
  this->pointHandler = new EditMeshPointHandler(this);
  this->setVisible(true);
}

EditMeshController::~EditMeshController() { delete this->pointHandler; }

void EditMeshController::paint(QPainter* painter,
                               const QStyleOptionGraphicsItem* option,
                               QWidget* widget) {
  Q_UNUSED(widget)
  Q_UNUSED(option)
  const auto& meshPoint = this->vertices;
  auto scale = painter->transform().m11();

  auto pen = QPen(Qt::black);
  pen.setWidth(1 / scale);
  painter->setBrush(QBrush(Qt::white));
  painter->setPen(pen);

  const auto& incident = this->incident;
  for (int i = 0; i < incident.size(); i += 3) {
    auto v1 = meshPoint[incident[i]];
    auto v2 = meshPoint[incident[i + 1]];
    auto v3 = meshPoint[incident[i + 2]];

    painter->drawLine(QPointF(v1.pos.x, v1.pos.y), QPointF(v2.pos.x, v2.pos.y));
    painter->drawLine(QPointF(v2.pos.x, v2.pos.y), QPointF(v3.pos.x, v3.pos.y));
    painter->drawLine(QPointF(v3.pos.x, v3.pos.y), QPointF(v1.pos.x, v1.pos.y));
  }

  for (const auto& point : meshPoint) {
    painter->drawEllipse(QPointF(point.pos.x, point.pos.y),
                         pointHandler->AbsolutePointRadius / scale,
                         pointHandler->AbsolutePointRadius / scale);
  }
  painter->setBrush(QBrush(Qt::red));
  for (const auto& i : selectIndex) {
    auto point = vertices[i];
    painter->drawEllipse(QPointF(point.pos.x, point.pos.y),
                         pointHandler->AbsolutePointRadius / scale,
                         pointHandler->AbsolutePointRadius / scale);
  }
}

QRectF EditMeshController::boundingRect() const {
  if (this->controllerParent != nullptr) {
    return controllerParent->boundingRect();
  }
  return {};
}

int EditMeshController::type() const {
  return ControllerType::EditMeshControllerType;
}

QPointF EditMeshController::localPointToScene(const QPointF& point) {
  return controllerParent->localPointToScene(point);
}

QPointF EditMeshController::scenePointToLocal(const QPointF& point) {
  return controllerParent->scenePointToLocal(point);
}

std::vector<QPointF> EditMeshController::getPointFromScene() {
  auto res = std::vector<QPointF>();
  for (const auto& vertex : this->vertices) {
    res.emplace_back(vertex.pos.x, vertex.pos.y);
  }
  return res;
}

void EditMeshController::selectAtScene(QRectF sceneRect) {
  AbstractController::selectAtScene(sceneRect);
  this->selectIndex.clear();
  for (int i = 0; i < vertices.size(); i++) {
    const auto& vertex = vertices[i];
    auto p = QPointF(vertex.pos.x, vertex.pos.y);
    if (sceneRect.contains(p)) {
      this->selectIndex.push_back(i);
    }
  }
  this->update();
}

void EditMeshController::setPointFromScene(int index,
                                           const QPointF& scenePosition) {
  // AbstractController::setPointFromScene(index, scenePosition);
  auto& data = this->vertices[index];

  // TODO: it should also update the incident and uv
  data.pos.x = scenePosition.x();
  data.pos.y = scenePosition.y();
  this->update();
}

void EditMeshController::selectPoint(int index) {
  this->selectIndex.push_back(index);
  this->update();
}

void EditMeshController::unSelectPoint() {
  this->selectIndex.clear();
  this->update();
}

void EditMeshController::addUndoCommand(QUndoCommand* command) {

  if (this->controllerParent->type() != RootControllerType) {
    delete command;
    return;
  }
  auto root = static_cast<RootController*>(controllerParent);
  root->pushUndoCommand(command);
}

void EditMeshController::setActiveSelectController(
    ActiveSelectController controller) {
  qDebug() << "change" << controller;
}
}  // namespace Scene
