#include "editmeshcontroller.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
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

  class MeshRotationUndoEvent : public Command::ControllerCommand {
    EditMeshRotationController* selectController;
    bool first = true;

   public:
    // two rotation record for controller
    double beforeRotation = 0;
    double afterRotation = 0;
    MeshRotationUndoEvent(EditMeshRotationController* selectController,
                          Scene::AbstractController* controller,
                          QUndoCommand* parent = nullptr)
        : ControllerCommand(controller, parent) {
      this->selectController = selectController;
    }
    void redo() override {
      if (first) {
        first = false;
        return;
      }
      ControllerCommand::redo();
      selectController->rotation = afterRotation;
    }

    void undo() override {
      ControllerCommand::undo();
      // the rotation will only affect the rotation controller, will not affect
      // the actual mesh point
      selectController->rotation = beforeRotation;
      selectController->update();
    }
  };
  friend MeshRotationUndoEvent;

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
    auto undoEvent = std::make_unique<MeshRotationUndoEvent>(this, editMesh);
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
    const auto& pList = editMesh->getPointFromScene();
    for (const auto& index : editMesh->getSelectIndex()) {
      auto pos = pList[index];
      undoEvent->addNewInfo({pos, index});
    }
    auto root = RootController::findRootController(editMesh);
    if (root != nullptr) {
      root->pushUndoCommand(undoEvent.release());
    }
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


/**
 * the pen controller bound rect is the edit mesh bound to receive all event from edit mesh
 */
class EditMeshPenController : public AbstractSelectController {
 private:
  EditMeshController* editMesh;
  class PenEventHandler: public PointEventHandler {
  private:
    EditMeshController* mesh;
  public:
    PenEventHandler(EditMeshController* controller): PointEventHandler(controller) {
      this->mesh = controller;
    }

    void pointPressedEvent(int index, QGraphicsSceneMouseEvent* event) override {
      if (index == -1) {
        mesh->addEditPoint(event->scenePos());
        mesh->upDateCDT();
      }
    }
  };


  friend PenEventHandler;
  std::unique_ptr<PenEventHandler> handler;
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
    handler->mousePressEvent(event);
    event->accept();
  }

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {
    handler->mouseMoveEvent(event);
  }
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override {
    handler->mouseReleaseEvent(event);
  }
 public:
  explicit EditMeshPenController(EditMeshController* editMesh): AbstractSelectController(editMesh) {
    this->editMesh = editMesh;
    this->handler = std::make_unique<PenEventHandler>(editMesh);
    this->setVisible(false);
  }

  QRectF boundingRect() const override {
    return editMesh->boundingRect();
  }
  void setBoundRect(const QRectF& rect) override{}
  void setBoundRect(const std::vector<QPointF>& pointList) override{}
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override{}
};

CDT::EdgeUSet EditMeshController::incidentToEdge(
    const std::vector<unsigned int>& incident) {
  auto pointPairMap = CDT::EdgeUSet();
   
   for (unsigned int i = 0; i < incident.size(); i += 3) {
    std::array<std::pair<unsigned int, unsigned int>, 3> lineArray = {
        std::make_pair(incident[i], incident[i + 1]),
        std::make_pair(incident[i + 1], incident[i + 2]),
        std::make_pair(incident[i + 2], incident[i]),
    };
    for (const auto& line : lineArray) {
      auto edge = CDT::Edge(line.first, line.second);
      if (!pointPairMap.contains(edge)) {
        pointPairMap.insert(edge);
      }
    }
  }
  return pointPairMap;
}

void EditMeshController::addEditPoint(
    const QPointF& scenePoint,bool select) {
  this->editPoint.emplace_back(scenePoint.x(), scenePoint.y());
  if (select) {
    this->selectIndex.clear();
    this->selectIndex.push_back(editPoint.size() - 1);
  }
  this->update();
}

void EditMeshController::addEditPoint(const QPointF& scenePoint,
                                      int lastSelectIndex, bool select) {
  addEditPoint(scenePoint, select);
  this->fixedEdge.insert(CDT::Edge(lastSelectIndex, editPoint.size() - 1));
  this->update();
}


void EditMeshController::upDateActiveTool() {
  auto pList = std::vector<QPointF>();
  for (int select_index : this->selectIndex) {
    pList.emplace_back(editPoint[select_index].x, editPoint[select_index].y);
  }
  selectControllerTool[activeSelectTool]->setBoundRect(pList);
}

void EditMeshController::upDateCDT() {
  CDT::Triangulation<float> cdt;
  cdt.insertVertices(editPoint);
  CDT::EdgeVec vec;
  for (const auto & p : this->fixedEdge) {
    vec.push_back(p);
  }
   
  cdt.insertEdges(vec);
  cdt.eraseSuperTriangle();

  this->editPoint = cdt.vertices;
  this->fixedEdge = cdt.fixedEdges;
  this->allEdge = CDT::extractEdgesFromTriangles(cdt.triangles);
  this->update();
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

void EditMeshController::keyPressEvent(QKeyEvent* event) {
  AbstractController::keyPressEvent(event);
  if (event->key() == Qt::Key_Delete) {
  }
}

EditMeshController::EditMeshController(
    const std::vector<MeshVertex>& vertices,
    const std::vector<unsigned int>& incident, QGraphicsItem* parent)
    : AbstractController(parent) {
  this->fixedEdge = this->incidentToEdge(incident);
  this->allEdge = fixedEdge;
  for (const auto& vertex : vertices) {
    this->editPoint.emplace_back(vertex.pos.x, vertex.pos.y);
  }

  this->pointHandler = new EditMeshPointHandler(this);
  this->setVisible(true);
  auto rectSelectController = new EditMeshActualRectController(this);
  auto rotationController = new EditMeshRotationController(this);
  auto penController = new EditMeshPenController(this);
  this->selectControllerTool[0] = rectSelectController;
  this->selectControllerTool[1] = rotationController;
  this->selectControllerTool[2] = penController;
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
  const auto& meshPoint = this->editPoint;
  auto scale = painter->transform().m11();

  auto pen = QPen(Qt::black);
  pen.setWidth(static_cast<int>(1 / scale));
  painter->setBrush(QBrush(Qt::white));
  painter->setPen(pen);

  for (const auto& edge : this->allEdge) {
    auto v1 = editPoint[edge.v1()];
    auto v2 = editPoint[edge.v2()];

    painter->drawLine(QPointF(v1.x, v1.y), QPointF(v2.x, v2.y));
  }

  for (const auto& point : meshPoint) {
    painter->drawEllipse(QPointF(point.x, point.y),
                         PointEventHandler::AbsolutePointRadius / scale,
                         PointEventHandler::AbsolutePointRadius / scale);
  }
  painter->setBrush(QBrush(Qt::red));
  for (const auto& i : selectIndex) {
    auto point = meshPoint[i];
    painter->drawEllipse(QPointF(point.x, point.y),
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
  for (const auto& vertex : this->editPoint) {
    res.emplace_back(vertex.x, vertex.y);
  }
  return res;
}

void EditMeshController::selectAtScene(QRectF sceneRect) {
  AbstractController::selectAtScene(sceneRect);
  this->selectIndex.clear();
  for (int i = 0; i < editPoint.size(); i++) {
    const auto& vertex = editPoint[i];
    auto p = QPointF(vertex.x, vertex.y);
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
  auto& data = this->editPoint[index];

  data.x = static_cast<float>(scenePosition.x());
  data.y = static_cast<float>(scenePosition.y());

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
  this->activeSelectTool = readyController;
  this->selectControllerTool[readyController]->show();
  upDateActiveTool();
}

std::vector<int> EditMeshController::getSelectIndex() const {
  return this->selectIndex;
}
}  // namespace Scene
