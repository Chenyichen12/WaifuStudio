#include "meshcontroller.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include "../command/controllercommand.h"
#include "mesh.h"
#include "meshselectcontrollers.h"
#include "pointeventhandler.h"
namespace Scene {
/**
 * actual mesh rect controller
 * should push command to undo command list
 */
class MeshActualRectController : public AbstractRectSelectController {
 private:
  MeshController* mesh;

 public:
  explicit MeshActualRectController(MeshController* controller)
      : AbstractRectSelectController(controller), mesh(controller) {
    this->setVisible(false);
  }

 protected:
  std::vector<int> getSelectIndex() override {
    return mesh->getSelectedPointIndex();
  }

  void pointsHaveMoved() override { mesh->upDateMeshBuffer(); }

  void rectEndMove(const QPointF& startPoint,
                   const QPointF& endPoint) override {
    auto undoEvent = std::make_unique<Command::MeshControllerCommand>(mesh);
    for (const auto& info : startPointPos) {
      undoEvent->addOldInfo(info);
    }
    for (const auto& index : mesh->getSelectedPointIndex()) {
      auto pos = mesh->getPointScenePosition(index);
      undoEvent->addNewInfo({pos, index});
    }
    auto root = RootController::findRootController(controller);
    if (root != nullptr) {
      root->pushUndoCommand(undoEvent.release());
    }
    AbstractRectSelectController::rectEndMove(startPoint, endPoint);
  }
};

/**
 * the actual rotationController for the mesh
 */
class MeshRotationSelectController : public AbstractRotationSelectController {
 private:
  /**
   * private class for the undo command for rotation controller
   */
  class MeshRotationUndoEvent : public Command::MeshControllerCommand {
    MeshRotationSelectController* selectController;

   public:
    // two rotation record for controller
    double beforeRotation = 0;
    double afterRotation = 0;
    MeshRotationUndoEvent(MeshRotationSelectController* selectController,
                          Scene::MeshController* controller,
                          QUndoCommand* parent = nullptr)
        : MeshControllerCommand(controller, parent) {
      this->selectController = selectController;
    }
    void redoNotFirst() override {
      MeshControllerCommand::redoNotFirst();
      selectController->rotation = afterRotation;
      selectController->update();
    }

    void undo() override {
      MeshControllerCommand::undo();
      // the rotation will only affect the rotation controller, will not affect
      // the actual mesh point
      selectController->rotation = beforeRotation;
      selectController->update();
    }
  };
  friend MeshRotationUndoEvent;

  MeshController* controller;

 public:
  explicit MeshRotationSelectController(MeshController* controller)
      : AbstractRotationSelectController(controller), controller(controller) {
    ifAutoMoveUpdate = false;
    this->setVisible(false);
  }

 protected:
  std::vector<int> getSelectIndex() override {
    return controller->getSelectedPointIndex();
  }
  // record the undo command
  void controllerEndDrag(const QPointF& mouseScenePos) override {
    Q_UNUSED(mouseScenePos)
    auto undoEvent = std::make_unique<MeshRotationUndoEvent>(this, controller);
    if (this->dragState == ROTATION) {
      undoEvent->beforeRotation = startDragRotation;
      undoEvent->afterRotation = this->rotation;
    } else {
      undoEvent->beforeRotation = this->rotation;
      undoEvent->afterRotation = this->rotation;
    }
    for (const auto& info : startPointPos) {
      undoEvent->addOldInfo(info);
    }
    for (const auto& index : controller->getSelectedPointIndex()) {
      auto pos = controller->getPointScenePosition(index);
      undoEvent->addNewInfo({pos, index});
    }
    auto root = RootController::findRootController(controller);
    if (root != nullptr) {
      root->pushUndoCommand(undoEvent.release());
    }
    AbstractRotationSelectController::controllerEndDrag(mouseScenePos);
  }
  void pointsHaveMoved() override { this->controller->upDateMeshBuffer(); }
};
class MeshController::MeshControllerEventHandler : public PointEventHandler {
 private:
  MeshController* mesh;
  QPointF startPoint;

 public:
  explicit MeshControllerEventHandler(MeshController* controller)
      : PointEventHandler(controller) {
    mesh = controller;
  }

 protected:
  void pointMoveEvent(int index, QGraphicsSceneMouseEvent* event) override {
    const auto& pos = event->scenePos();
    if (index == -1) {
      return;
    }
    mesh->setPointFromScene(index, pos);
    mesh->upDateMeshBuffer();
  }
  void pointPressedEvent(int index, QGraphicsSceneMouseEvent* event) override {
    if (index == -1) {
      mesh->unSelectPoint();
    } else {
      if (event->modifiers() != Qt::ShiftModifier) {
        mesh->unSelectPoint();
      }
      mesh->selectPoint(index);
      startPoint = mesh->getPointFromScene()[index];
    }
  }
  void pointReleaseEvent(QGraphicsSceneMouseEvent* event) override {
    if (this->currentIndex == -1) {
      return;
    }

    // push to undostack
    auto undoCommand = std::make_unique<Command::MeshControllerCommand>(mesh);
    auto oldPoint = startPoint;
    undoCommand->addOldInfo({oldPoint, this->currentIndex});

    undoCommand->addNewInfo({event->scenePos(), this->currentIndex});
    auto root = RootController::findRootController(mesh);
    if (root != nullptr) {
      root->pushUndoCommand(undoCommand.release());
    }
  }
};

void MeshController::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  handler->mousePressEvent(event);
  // AbstractController::mousePressEvent(event);
}

void MeshController::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  handler->mouseMoveEvent(event);
  AbstractController::mouseMoveEvent(event);
}

// handle release
void MeshController::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  handler->mouseReleaseEvent(event);
  AbstractController::mouseReleaseEvent(event);
}

void MeshController::unSelectTheController() {
  AbstractController::unSelectTheController();
  this->unSelectPoint();
}

const std::vector<bool>& MeshController::getSelectedTable() const {
  return this->selectedPoint;
}

std::vector<int> MeshController::getSelectedPointIndex() const {
  auto res = std::vector<int>();
  for (int i = 0; i < this->selectedPoint.size(); i++) {
    if (selectedPoint[i]) {
      res.push_back(i);
    }
  }
  return res;
}

QPointF MeshController::getPointScenePosition(int index) const {
  auto pos = this->controlMesh->getVertices()[index].pos;
  return {pos.x, pos.y};
}

std::vector<QPointF> MeshController::getPointFromScene() {
  auto res = std::vector<QPointF>();
  for (const auto& vertex : this->controlMesh->getVertices()) {
    res.emplace_back(vertex.pos.x, vertex.pos.y);
  }
  return res;
}

std::vector<QPointF> MeshController::getSelectedPointScenePosition() const {
  std::vector<QPointF> result;
  for (int i = 0; i < selectedPoint.size(); ++i) {
    if (selectedPoint[i]) {
      const auto& point = controlMesh->getVertices()[i].pos;
      result.emplace_back(point.x, point.y);
    }
  }
  return result;
}

MeshController::MeshController(Mesh* controlMesh, QGraphicsItem* parent)
    : AbstractController(parent) {
  this->controlMesh = controlMesh;
  // default false for selected point
  this->selectedPoint =
      std::vector<bool>(controlMesh->getVertices().size(), false);
  handler = new MeshControllerEventHandler(this);

  // setup select rect item
  auto rectSelectController = new MeshActualRectController(this);
  this->selectControllerList[0] = rectSelectController;

  auto rotationController = new MeshRotationSelectController(this);
  this->selectControllerList[1] = rotationController;

  auto r = RootController::findRootController(this);
  if (r != nullptr) {
    rectSelectController->setPadding(r->boundingRect().width() / 100);
    rectSelectController->setLineWidth(r->boundingRect().width() / 300);
    rotationController->setRadius(r->boundingRect().width() / 150);
    rotationController->setLineLength(r->boundingRect().width() / 15);
  }
  this->MeshController::setActiveSelectController(RectController);
}

void MeshController::setActiveSelectController(
    ActiveSelectController controller) {
  int readyController = controller;
  if (controller == PenController) {
    readyController = 0;
  }
  this->selectControllerList[activeSelectController]->setVisible(false);
  this->activeSelectController = readyController;

  this->selectControllerList[activeSelectController]->setVisible(true);
  const auto& vec = getSelectedPointScenePosition();
  this->selectControllerList[activeSelectController]->setBoundRect(vec);
}

QRectF MeshController::boundingRect() const {
  // normally this project should only have one view
  auto view = this->scene()->views().first();

  auto r = RootController::findRootController(this);
  double betterPadding = 10;
  if (r != nullptr) {
    betterPadding = r->boundingRect().width() / 300;
  }
  return controlMesh->boundingRect().marginsAdded(
      QMarginsF(betterPadding, betterPadding, betterPadding, betterPadding));
}

void MeshController::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  const auto& meshPoint = controlMesh->getVertices();
  auto scale = painter->transform().m11();

  auto pen = QPen(Qt::black);
  pen.setWidth(1 / scale);
  painter->setBrush(QBrush(Qt::white));
  painter->setPen(pen);

  const auto& incident = controlMesh->getIncident();
  for (int i = 0; i < incident.size(); i += 3) {
    auto v1 = meshPoint[incident[i]];
    auto v2 = meshPoint[incident[i + 1]];
    auto v3 = meshPoint[incident[i + 2]];

    painter->drawLine(QPointF(v1.pos.x, v1.pos.y), QPointF(v2.pos.x, v2.pos.y));
    painter->drawLine(QPointF(v2.pos.x, v2.pos.y), QPointF(v3.pos.x, v3.pos.y));
    painter->drawLine(QPointF(v3.pos.x, v3.pos.y), QPointF(v1.pos.x, v1.pos.y));
  }

  for (int i = 0; i < selectedPoint.size(); ++i) {
    auto& point = meshPoint[i];
    if (selectedPoint[i]) {
      painter->setBrush(QBrush(Qt::red));
    } else {
      painter->setBrush(QBrush(Qt::white));
    }
    painter->drawEllipse(QPointF(point.pos.x, point.pos.y),
                         handler->AbsolutePointRadius / scale,
                         handler->AbsolutePointRadius / scale);
  }
}

int MeshController::controllerId() { return controlMesh->getLayerId(); }

int MeshController::type() const { return ControllerType::MeshControllerType; }

QPointF MeshController::localPointToScene(const QPointF& point) {
  return controllerParent->localPointToScene(point);
}

QPointF MeshController::scenePointToLocal(const QPointF& point) {
  return controllerParent->scenePointToLocal(point);
}

void MeshController::unSelectPoint() {
  for (auto&& selected_point : this->selectedPoint) {
    selected_point = false;
  }
  this->selectControllerList[activeSelectController]->setBoundRect(QRectF());
  this->update();
}

void MeshController::selectPoint(int index) {
  if (index < 0 || index >= selectedPoint.size()) {
    return;
  }
  this->selectedPoint[index] = true;
  const auto& vec = getSelectedPointScenePosition();
  this->selectControllerList[activeSelectController]->setBoundRect(vec);
  this->update();
}

void MeshController::upDateMeshBuffer() const {
  this->controlMesh->upDateBuffer();
}

void MeshController::setPointFromScene(int index,
                                       const QPointF& scenePosition) {
  auto vec = controlMesh->getVertices()[index];
  vec.pos.x = scenePosition.x();
  vec.pos.y = scenePosition.y();
  controlMesh->setVerticesAt(index, vec);

  // update the select rect if needed
  const auto& selectVec = getSelectedPointScenePosition();
  const auto& rect = AbstractSelectController::boundRectFromPoints(selectVec);
  this->selectControllerList[activeSelectController]->setBoundRect(rect);

  this->update();
}

void MeshController::setPointFromLocal(int index,
                                       const QPointF& localPosition) {
  auto scenePos = localPointToScene(localPosition);
  setPointFromScene(index, scenePos);
}
void MeshController::selectAtScene(QRectF sceneRect) {
  AbstractController::selectAtScene(sceneRect);
  this->unSelectPoint();
  for (int i = 0; i < controlMesh->getVertices().size(); ++i) {
    const auto& point = controlMesh->getVertices()[i].pos;
    auto qPoint = QPointF(point.x, point.y);
    if (sceneRect.contains(qPoint)) {
      selectPoint(i);
    }
  }
}
MeshController::~MeshController() { delete handler; }
}  // namespace Scene