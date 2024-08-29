#include "editmeshcontroller.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "../command/controllercommand.h"
#include "meshselectcontrollers.h"
#include "pointeventhandler.h"
#include "scenecontroller.h"
namespace Scene {

class EditMeshPointUndoCommand : public Command::ControllerCommand {
  bool first = true;

 public:
  explicit EditMeshPointUndoCommand(EditMeshController* controller,
                                    QUndoCommand* parent = nullptr)
      : ControllerCommand(controller, parent) {}

  void redo() override {
    if (first) {
      first = false;
      return;
    }

    ControllerCommand::redo();
  }
};

/**
 * edit mesh actual select controller
 * when multiselect 2 or more point
 * it will show a rect that can controller the multipoint
 * also handle undo command
 */
class EditMeshActualRectController : public AbstractRectSelectController {
 private:
  EditMeshController* editMesh;

 public:
  explicit EditMeshActualRectController(EditMeshController* controller)
      : AbstractRectSelectController(controller), editMesh(controller) {}

 protected:
  std::vector<int> getSelectIndex() override {
    return editMesh->getSelectIndex();
  }

  // push to undo command
  void rectEndMove(const QPointF& startPoint,
                   const QPointF& endPoint) override {
    auto undoCommand = std::make_unique<EditMeshPointUndoCommand>(editMesh);
    for (const auto& startCommand : startPointPos) {
      undoCommand->addOldInfo({startCommand.p, startCommand.index});
    }

    const auto& pList = editMesh->getPointFromScene();
    for (int select_index : editMesh->getSelectIndex()) {
      auto p = pList[select_index];
      undoCommand->addNewInfo({p, select_index});
    }

    editMesh->addUndoCommand(undoCommand.release());
    AbstractRectSelectController::rectEndMove(startPoint, endPoint);
  }
};

class EditMeshRotationController : public AbstractRotationSelectController {
 private:
  EditMeshController* editMesh;

 public:
  explicit EditMeshRotationController(EditMeshController* controller)
      : AbstractRotationSelectController(controller) {
    this->editMesh = controller;
  }

 protected:
  std::vector<int> getSelectIndex() override {
    return editMesh->getSelectIndex();
  };

  void controllerEndDrag(const QPointF& mouseScenePos) override {
    AbstractRotationSelectController::controllerEndDrag(mouseScenePos);
  }
};

/**
 * edit mesh point handler
 * drag the point and handle the press event
 */
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

void EditMeshController::upDateActiveTool() {
  if (this->selectIndex.size() <= 1) {
    selectControllerTool[activeSelectTool]->setVisible(false);
    return;
  }
  auto pList = std::vector<QPointF>();
  for (int select_index : this->selectIndex) {
    pList.emplace_back(vertices[select_index].pos.x,
                       vertices[select_index].pos.y);
  }
  selectControllerTool[activeSelectTool]->setBoundRect(
      AbstractSelectController::boundRectFromPoints(pList));
  selectControllerTool[activeSelectTool]->setVisible(true);
}

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
  auto rectSelectController = new EditMeshActualRectController(this);
  auto rotationController = new EditMeshRotationController(this);
  this->selectControllerTool[0] = rectSelectController;
  this->selectControllerTool[1] = rotationController;
  auto r = RootController::findRootController(this);
  if (r != nullptr) {
    rectSelectController->setPadding(r->boundingRect().width() / 100);
    rectSelectController->setLineWidth(r->boundingRect().width() / 300);
    rotationController->setRadius(r->boundingRect().width() / 150);
    rotationController->setLineLength(r->boundingRect().width() / 15);
  }
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
                         PointEventHandler::AbsolutePointRadius / scale,
                         PointEventHandler::AbsolutePointRadius / scale);
  }
  painter->setBrush(QBrush(Qt::red));
  for (const auto& i : selectIndex) {
    auto point = vertices[i];
    painter->drawEllipse(QPointF(point.pos.x, point.pos.y),
                         PointEventHandler::AbsolutePointRadius / scale,
                         PointEventHandler::AbsolutePointRadius / scale);
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

  upDateActiveTool();
  this->update();
}

void EditMeshController::setPointFromScene(int index,
                                           const QPointF& scenePosition) {
  // AbstractController::setPointFromScene(index, scenePosition);
  auto& data = this->vertices[index];

  data.pos.x = scenePosition.x();
  data.pos.y = scenePosition.y();

  upDateActiveTool();
  this->update();
}

void EditMeshController::selectPoint(int index) {
  this->selectIndex.push_back(index);
  upDateActiveTool();
  this->update();
}

void EditMeshController::unSelectPoint() {
  this->selectIndex.clear();
  upDateActiveTool();
  this->update();
}

void EditMeshController::addUndoCommand(QUndoCommand* command) const {
  if (this->controllerParent->type() != RootControllerType) {
    delete command;
    return;
  }
  auto root = static_cast<RootController*>(controllerParent);
  root->pushUndoCommand(command);
}

void EditMeshController::setActiveSelectController(
    ActiveSelectController controller) {
  auto readyController = controller;
  this->selectControllerTool[activeSelectTool]->hide();
  if (controller == PenController) {
    readyController = RectController;
  }
  this->activeSelectTool = readyController;
  this->selectControllerTool[readyController]->show();
  upDateActiveTool();
}

std::vector<int> EditMeshController::getSelectIndex() const {
  return this->selectIndex;
}
}  // namespace Scene
