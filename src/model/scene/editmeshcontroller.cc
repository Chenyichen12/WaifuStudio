#include "editmeshcontroller.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include "../command/controllercommand.h"
#include "../command/editcontrollercommand.h"
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
      : AbstractRectSelectController(controller), editMesh(controller) {
    this->setVisible(false);
  }

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
    this->setVisible(false);
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

  /**
   * to find the length of point to lien
   * @param point test point
   * @param line test line
   * @return distance
   */
  static float lengthFromPointToLine(const QPointF& point, const QLineF& line) {
    // vector va = point - line.p1()
    QPointF va = point - line.p1();
    // vector vb = line.p2() - line.p1()
    QPointF vb = line.p2() - line.p1();

    // calculate vb length
    float vbLength = std::sqrt(vb.x() * vb.x() + vb.y() * vb.y());
    float projectionLength = (va.x() * vb.x() + va.y() * vb.y()) / vbLength;
    QPointF projectionPoint = line.p1() + (projectionLength / vbLength) * vb;
    float distance = std::sqrt(std::pow(point.x() - projectionPoint.x(), 2) +
                               std::pow(point.y() - projectionPoint.y(), 2));

    return distance;
  }

  /**
   * find the handler if hit one edge
   * @param scenePoint position
   * @param width edge width
   * @return hit edge. if no hit return nullopt
   */
  std::optional<CDT::Edge> ifHitFixedEdge(const QPointF& scenePoint,
                                          int width = 4) const {
    const auto& fixedEdge = controller->getFixedEdge();
    const auto& pointList = controller->getPointFromScene();
    for (const auto& edge : fixedEdge) {
      auto p1 = pointList[edge.v1()];
      auto p2 = pointList[edge.v2()];
      float d = lengthFromPointToLine(scenePoint, {p1, p2});
      if (d < width) {
        return edge;
      }
    }
    return std::nullopt;
  }

 protected:
  void pointMoveEvent(int current, QGraphicsSceneMouseEvent* event) override {
    if (current == -1) {
      return;
    }
    controller->setPointFromScene(current, event->scenePos());
  }
  void pointPressedEvent(int index, QGraphicsSceneMouseEvent* event) override {
    // if modifier is not shift should unselect all select item
    if (event->modifiers() != Qt::ShiftModifier) {
      controller->unSelectPoint();
      controller->unSelectFixedEdge();
    }
    if (index == -1) {
      auto scale = getScaleFromTheView(controller->scene(), event->widget());
      auto edge = this->ifHitFixedEdge(event->scenePos(), 3.0 / scale);
      if (edge.has_value()) {
        // select fix edge
        controller->selectFixedEdge(edge.value());
      }
    } else {
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
 * the pen controller bound rect is the edit mesh bound to receive all event
 * from edit mesh
 */
class EditMeshPenController : public AbstractSelectController {
 private:
  EditMeshController* editMesh;
  class PenEventHandler : public PointEventHandler {
   private:
    EditMeshController* mesh;
    std::unique_ptr<Command::EditMeshControllerCommand> eventCommand;

   public:
    PenEventHandler(EditMeshController* controller)
        : PointEventHandler(controller) {
      this->mesh = controller;
    }

    void pointPressedEvent(int index,
                           QGraphicsSceneMouseEvent* event) override {
      const auto& selectIndex = mesh->getSelectIndex();
      // handle the no index situation
      // aim to add point and fixed edge
      eventCommand = std::make_unique<Command::EditMeshControllerCommand>(mesh);
      eventCommand->recordOldState();
      if (index == -1) {
        if (selectIndex.size() == 1) {
          mesh->addEditPoint(event->scenePos(), selectIndex[0]);
          mesh->upDateCDT();
        } else {
          mesh->addEditPoint(event->scenePos());
          mesh->upDateCDT();
        }
      }
      // aim to connect two point
      else {
        if (!selectIndex.empty() && selectIndex[0] != index) {
          mesh->connectFixedEdge(selectIndex[0], index);
          mesh->upDateCDT();
        } else {
          mesh->selectPoint(index);
        }
      }
    }
    void pointMoveEvent(int current, QGraphicsSceneMouseEvent* event) override {
      if (current == -1) return;
      mesh->setPointFromScene(current, event->scenePos());
    }
    void pointReleaseEvent(QGraphicsSceneMouseEvent* event) override {
      if (this->eventCommand != nullptr) {
        eventCommand->recordNewState();
        mesh->addUndoCommand(eventCommand.release());
        eventCommand.reset(nullptr);
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
  explicit EditMeshPenController(EditMeshController* editMesh)
      : AbstractSelectController(editMesh) {
    this->editMesh = editMesh;
    this->handler = std::make_unique<PenEventHandler>(editMesh);
    this->setVisible(false);
  }

  QRectF boundingRect() const override { return editMesh->boundingRect(); }
  void setBoundRect(const QRectF& rect) override {}
  void setBoundRect(const std::vector<QPointF>& pointList) override {}
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override {}
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

void EditMeshController::addEditPoint(const QPointF& scenePoint, bool select) {
  this->editPoint.emplace_back(scenePoint.x(), scenePoint.y());
  if (select) {
    this->unSelectPoint();
    this->selectPoint(editPoint.size() - 1);
  }
  this->update();
}

void EditMeshController::addEditPoint(const QPointF& scenePoint,
                                      int lastSelectIndex, bool select) {
  addEditPoint(scenePoint, select);
  this->fixedEdge.insert(CDT::Edge(lastSelectIndex, editPoint.size() - 1));
  this->update();
}

void EditMeshController::connectFixedEdge(int index1, int index2,
                                          bool selectIndex2) {
  auto edge = CDT::Edge(index1, index2);
  this->fixedEdge.insert(edge);
  if (selectIndex2) {
    this->selectIndex.clear();
    this->selectIndex.insert(index2);
  }
  this->update();
}

/**
 * the actual remove point with edge method
 * @param index the point to remove
 * @param allEdge edge container
 * @param newAllEdge new edge result
 */
void doRemovePoint(int index, const CDT::EdgeUSet& allEdge,
                   CDT::EdgeUSet& newAllEdge) {
  newAllEdge.clear();
  for (auto& edge : allEdge) {
    if (edge.v1() == index || edge.v2() == index) {
      continue;
    }
    if (edge.v1() < index && edge.v2() < index) {
      // addToEdgeSet(edge,edge);
      newAllEdge.insert(edge);
      continue;
    }
    auto i1 = edge.v1() > index ? edge.v1() - 1 : edge.v1();
    auto i2 = edge.v2() > index ? edge.v2() - 1 : edge.v2();
    newAllEdge.insert({i1, i2});
  }
}

void EditMeshController::removePoint(int index, bool withEdge) {
  if (!withEdge) {
    this->editPoint.erase(editPoint.begin() + index);
    this->update();
    return;
  }

  CDT::EdgeUSet newAllEdge;
  CDT::EdgeUSet newFixedEdge;

  doRemovePoint(index, this->allEdge, newAllEdge);
  doRemovePoint(index, this->fixedEdge, newFixedEdge);

  this->editPoint.erase(editPoint.begin() + index);
  this->fixedEdge = newFixedEdge;
  this->allEdge = newAllEdge;
  this->update();
}

void EditMeshController::removePoints(std::vector<int> index, bool withEdge) {
  std::ranges::sort(index, std::greater<>());
  for (int value : index) {
    removePoint(value, withEdge);
  }
}

void EditMeshController::removeFixedEdge(int index1, int index2) {
  auto edge = CDT::Edge(index1, index2);
  removeFixedEdge(edge);
}

void EditMeshController::removeFixedEdge(const CDT::Edge& edge) {
  if (fixedEdge.contains(edge)) {
    fixedEdge.erase(edge);
    update();
  }
}

bool EditMeshController::ifValidTriangle() const {
  if (editPoint.empty()) {
    return false;
  }
  CDT::Triangulation<float> cdt;
  cdt.insertVertices(editPoint);
  CDT::EdgeVec vec;
  vec.assign(fixedEdge.begin(), fixedEdge.end());
  try {
    cdt.insertEdges(vec);
  } catch (...) {
    return false;
  }

  cdt.eraseOuterTriangles();
  std::unordered_set<unsigned int> triIndex;

  for (const auto& triangle : cdt.triangles) {
    const auto& v = triangle.vertices;
    triIndex.insert(v[0]);
    triIndex.insert(v[1]);
    triIndex.insert(v[2]);
  }
  if (triIndex.size() != editPoint.size()) {
    return false;
  }

  return true;
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
  for (const auto& p : this->fixedEdge) {
    vec.push_back(p);
  }
  // may throw the not allowable error
  // the cdt will auto resolve the error
  // and make the mesh allowable
  try {
    cdt.insertEdges(vec);
  } catch (...) {
    // TODO: remind error
    qDebug() << "cdt error";
  }

  cdt.eraseOuterTriangles();

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
    // delete select point and record undo command
    // may need to be abstract
    auto command = std::make_unique<Command::EditMeshControllerCommand>(this);
    command->recordOldState();

    // should first remove edge of the edit mesh
    // in order to remove right index
    for (const auto& edge : this->selectedFixEdge) {
      removeFixedEdge(edge);
    }
    auto selectVec = std::vector<int>();
    selectVec.assign(selectIndex.begin(), selectIndex.end());
    removePoints(selectVec);
    this->upDateCDT();
    this->unSelectPoint();
    this->unSelectFixedEdge();

    command->recordNewState();
    this->addUndoCommand(command.release());
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

  this->setFlag(ItemIsFocusable, true);
  this->grabKeyboard();
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

  painter->save();
  for (const auto& edge : this->fixedEdge) {
    auto v1 = editPoint[edge.v1()];
    auto v2 = editPoint[edge.v2()];
    painter->drawLine(QPointF(v1.x, v1.y), QPointF(v2.x, v2.y));
  }

  // draw the select edge
  auto selectEdgePen = QPen(Qt::green);
  selectEdgePen.setWidth(static_cast<int>(1 / scale));
  painter->setPen(selectEdgePen);
  for (const auto& edge : this->selectedFixEdge) {
    auto v1 = editPoint[edge.v1()];
    auto v2 = editPoint[edge.v2()];
    painter->drawLine(QPointF(v1.x, v1.y), QPointF(v2.x, v2.y));
  }

  // the notFixed edge use transparent pen
  auto noFixedPen = QPen(Qt::black);
  noFixedPen.setWidth(static_cast<int>(1 / scale));
  noFixedPen.setColor(QColor(255, 255, 255, 128));
  painter->setPen(noFixedPen);
  for (const auto& edge : this->allEdge) {
    auto v1 = editPoint[edge.v1()];
    auto v2 = editPoint[edge.v2()];
    if (!fixedEdge.contains(edge)) {
      painter->drawLine(QPointF(v1.x, v1.y), QPointF(v2.x, v2.y));
    }
  }

  painter->restore();
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
      this->selectIndex.insert(i);
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
  // select only smaller than size
  if (this->editPoint.size() > index) {
    this->selectIndex.insert(index);
    upDateActiveTool();
    this->update();
  }
}

void EditMeshController::unSelectPoint() {
  this->selectIndex.clear();
  upDateActiveTool();
  this->update();
}

void EditMeshController::selectFixedEdge(const CDT::Edge& fixed_edge) {
  if (this->fixedEdge.contains(fixed_edge)) {
    this->selectedFixEdge.insert(fixed_edge);
    update();
  }
}

void EditMeshController::unSelectFixedEdge() {
  this->selectedFixEdge.clear();
  update();
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
  return {selectIndex.begin(), selectIndex.end()};
}

std::vector<CDT::Edge> EditMeshController::getSelectedEdge() const {
  return {selectedFixEdge.begin(), selectedFixEdge.end()};
}

const CDT::EdgeUSet& EditMeshController::getFixedEdge() const {
  return this->fixedEdge;
}

const CDT::EdgeUSet& EditMeshController::getAllEdge() const {
  return this->allEdge;
}

void EditMeshController::setEditMesh(const std::vector<QPointF>& points,
                                     const CDT::EdgeUSet& fixedEdge,
                                     const CDT::EdgeUSet& allEdge) {
  this->unSelectPoint();
  this->editPoint.clear();
  for (const auto& point : points) {
    editPoint.emplace_back(point.x(), point.y());
  }

  this->fixedEdge = fixedEdge;
  this->allEdge = allEdge;
  update();
}
}  // namespace Scene
