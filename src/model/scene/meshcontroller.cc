#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include "../command/controllercommand.h"
#include "mesh.h"
#include "rectselectcontroller.h"
#include "scenecontroller.h"
namespace Scene {
RootController* findRootController(const QGraphicsItem* controller) {
  auto parent = controller->parentItem();
  while (parent != nullptr) {
    if (parent->type() == ControllerType::RootControllerType) {
      return static_cast<RootController*>(parent);
    }
    parent = parent->parentItem();
  }
  return nullptr;
}
double getScaleFromTheView(const QGraphicsScene* whichScene,
                           const QWidget* whichWidget) {
  for (const auto& view : whichScene->views()) {
    if (view->viewport() == whichWidget) {
      return view->transform().m11();
    }
  }
  return 1;
}

/**
 * handle the rect select of the mesh controller
 * it should change the position of the mesh point and push it to undo stack
 */
class MeshRectSelectController : public RectSelectController {
 private:
  MeshController* controller;
  std::vector<Command::ControllerCommandInfo> startCommandInfo;
  QRectF startDragRect;

 protected:
  void rectEndMove(const QPointF& startPoint,
                   const QPointF& endPoint) override {
    auto undoEvent =
        std::make_unique<Command::MeshControllerCommand>(controller);
    for (const auto& info : startCommandInfo) {
      undoEvent->addOldInfo(info);
    }
    for (const auto& index : controller->getSelectedPointIndex()) {
      auto pos = controller->getPointScenePosition(index);
      undoEvent->addNewInfo({pos, index});
    }
    auto root = findRootController(controller);
    if (root != nullptr) {
      root->pushUndoCommand(undoEvent.release());
    }
    startDragRect = QRectF();
    startCommandInfo.clear();
  }
  void rectMoving(const QPointF& pre, const QPointF& aft) override {
    auto delta = aft - pre;
    if (dragState == CENTER) {
      for (int index : controller->getSelectedPointIndex()) {
        auto pos = controller->getPointScenePosition(index);
        pos += delta;
        controller->setPointFromScene(index, pos);
      }
      controller->upDateMeshBuffer();
      return;
    }
    switch (dragState) {
      case TOP: {
        QPointF p = aft;
        p.setX(startDragRect.left());
        this->leftTopDrag(p);
        break;
      }
      case LEFT: {
        QPointF p = aft;
        p.setY(startDragRect.top());
        this->leftTopDrag(p);
        break;
      }
      case RIGHT: {
        QPointF p = aft;
        p.setY(startDragRect.top());
        this->rightTopDrag(p);
        break;
      }
      case BUTTON: {
        QPointF p = aft;
        p.setX(startDragRect.left());
        this->leftBottomDrag(p);
        break;
      }
      case LEFT_TOP:
        leftTopDrag(aft);
        break;

      case LEFT_BUTTON:
        leftBottomDrag(aft);
        break;
      case RIGHT_TOP:
        rightTopDrag(aft);
        break;
      case RIGHT_BUTTON:
        rightBottomDrag(aft);
        break;
      default:
        break;
    }
    controller->upDateMeshBuffer();
  }

  /**
   * drag handler
   * the mesh is make up with 9 controller point
   *
   * @param aft drag mouse position
   */
  void leftTopDrag(const QPointF& aft) {
    bool isXFlip = aft.x() > startDragRect.right();
    bool isYFlip = aft.y() > startDragRect.bottom();
    QPointF topLeft;
    QPointF bottomRight;
    if (isXFlip) {
      topLeft.setX(startDragRect.right());
      bottomRight.setX(aft.x());
    } else {
      topLeft.setX(aft.x());
      bottomRight.setX(startDragRect.right());
    }

    if (isYFlip) {
      topLeft.setY(startDragRect.bottom());
      bottomRight.setY(aft.y());
    } else {
      topLeft.setY(aft.y());
      bottomRight.setY(startDragRect.bottom());
    }

    handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
  }
  void leftBottomDrag(const QPointF& aft) {
    bool isXFlip = aft.x() > startDragRect.right();
    bool isYFlip = aft.y() < startDragRect.top();
    QPointF topLeft;
    QPointF bottomRight;
    if (isXFlip) {
      topLeft.setX(startDragRect.right());
      bottomRight.setX(aft.x());
    } else {
      topLeft.setX(aft.x());
      bottomRight.setX(startDragRect.right());
    }

    if (isYFlip) {
      topLeft.setY(aft.y());
      bottomRight.setY(startDragRect.top());
    } else {
      topLeft.setY(startDragRect.top());
      bottomRight.setY(aft.y());
    }

    handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
  }

  void rightTopDrag(const QPointF& aft) {
    bool isXFlip = aft.x() < startDragRect.left();
    bool isYFlip = aft.y() > startDragRect.bottom();
    QPointF topLeft;
    QPointF bottomRight;
    if (isXFlip) {
      topLeft.setX(aft.x());
      bottomRight.setX(startDragRect.left());
    } else {
      topLeft.setX(startDragRect.left());
      bottomRight.setX(aft.x());
    }

    if (isYFlip) {
      topLeft.setY(startDragRect.bottom());
      bottomRight.setY(aft.y());
    } else {
      topLeft.setY(aft.y());
      bottomRight.setY(startDragRect.bottom());
    }

    handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
  }

  void rightBottomDrag(const QPointF& aft) {
    bool isXFlip = aft.x() < startDragRect.left();
    bool isYFlip = aft.y() < startDragRect.top();
    QPointF topLeft;
    QPointF bottomRight;
    if (isXFlip) {
      topLeft.setX(aft.x());
      bottomRight.setX(startDragRect.left());
    } else {
      topLeft.setX(startDragRect.left());
      bottomRight.setX(aft.x());
    }

    if (isYFlip) {
      topLeft.setY(aft.y());
      bottomRight.setY(startDragRect.top());
    } else {
      topLeft.setY(startDragRect.top());
      bottomRight.setY(aft.y());
    }

    handleMouseAt(QRectF(topLeft, bottomRight), isXFlip, isYFlip);
  }

  // ReSharper disable once CppMemberFunctionMayBeConst
  void handleMouseAt(const QRectF& aftRect, bool isXFlip, bool isYFlip) {
    for (const auto& startPoint : this->startCommandInfo) {
      float u = (startPoint.p.x() - startDragRect.x()) / startDragRect.width();
      float v = (startPoint.p.y() - startDragRect.y()) / startDragRect.height();

      float x;
      float y;
      if (isXFlip) {
        x = aftRect.right() - aftRect.width() * u;
      } else {
        x = aftRect.width() * u + aftRect.left();
        // float y = aftRect.height() * v + aftRect.y();
      }
      if (isYFlip) {
        y = aftRect.bottom() - aftRect.height() * v;
      } else {
        y = aftRect.height() * v + aftRect.y();
      }
      controller->setPointFromScene(startPoint.index, QPointF(x, y));
    }
  }

  void rectStartMove(const QPointF& pos) override {
    for (int index : controller->getSelectedPointIndex()) {
      auto pos = controller->getPointScenePosition(index);
      startCommandInfo.push_back({pos, index});
    }
    startDragRect = this->boundRect;
  }

 public:
  MeshRectSelectController(MeshController* controller)
      : RectSelectController(controller), controller(controller) {}
};

class MeshController::MeshControllerEventHandler {
 private:
  MeshController* controller;
  int currentPressedIndex = -1;
  QPointF startPos;  // record the start drag pos

  /**
   * check if the testPoint circle is close enough to another point
   * @param x1 firstPoint
   * @param y1
   * @param x2 secondPoint
   * @param y2
   * @param scale which is the scale of the event view
   * used to define the testPoint is close enough
   * @return
   */
  bool contain(float x1, float y1, float x2, float y2, double scale) {
    // normally one view in a project
    float length = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    auto testRadius = AbsolutePointRadius / scale;
    return length < testRadius * testRadius;
  }

  /**
   * find the controller mesh point index contain the testPoint
   * @param x testPoint
   * @param y
   * @param scale testLength
   * @return if not found return nullopt
   */
  std::optional<int> gabPointAt(float x, float y, double scale) {
    const auto& vec = controller->controlMesh->getVertices();
    for (int i = 0; i < vec.size(); ++i) {
      const auto& testPoint = vec[i].pos;
      if (contain(x, y, testPoint.x, testPoint.y, scale)) {
        return i;
      }
    }
    return std::nullopt;
  }

 public:
  static constexpr float AbsolutePointRadius = 3;

  MeshControllerEventHandler(MeshController* controller) {
    this->controller = controller;
  }
  void mousePressEvent(QGraphicsSceneMouseEvent* event) {
    const auto& pos = event->scenePos();
    double scale = getScaleFromTheView(controller->scene(), event->widget());
    auto index = gabPointAt(pos.x(), pos.y(), scale);
    if (index.has_value()) {
      if (event->modifiers() != Qt::ShiftModifier) {
        controller->unSelectPoint();
      }

      controller->selectPoint(index.value());
      currentPressedIndex = index.value();
      startPos = pos;  // start drag
      event->accept();
    } else {
      controller->unSelectPoint();
      currentPressedIndex = -1;
      event->ignore();
    }
  }

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    const auto& pos = event->scenePos();
    if (currentPressedIndex == -1) {
      return;
    }
    controller->setPointFromScene(currentPressedIndex, pos);
    controller->upDateMeshBuffer();
  }

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (currentPressedIndex == -1) {
      return;
    }

    // push to undostack
    auto undoCommand =
        std::make_unique<Command::MeshControllerCommand>(controller);
    auto oldPoint = startPos;
    undoCommand->addOldInfo({oldPoint, currentPressedIndex});

    undoCommand->addNewInfo({event->scenePos(), currentPressedIndex});
    auto root = findRootController(controller);
    if (root != nullptr) {
      root->pushUndoCommand(undoCommand.release());
    }

    currentPressedIndex = -1;
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
  auto rectSelectController = new MeshRectSelectController(this);
  this->selectControllerList[0] = rectSelectController;
  rectSelectController->ifAutoMoveUpdate = false;
  auto r = findRootController(this);
  if (r != nullptr) {
    rectSelectController->setPadding(r->boundingRect().width() / 100);
    rectSelectController->setLineWidth(r->boundingRect().width() / 300);
  }
}

QRectF MeshController::boundingRect() const {
  // normally this project should only have one view
  auto view = this->scene()->views().first();

  auto r = findRootController(this);
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
    painter->drawEllipse(QPointF(point.pos.x, point.pos.y), 3 / scale,
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
  const auto& rect = RectSelectController::boundRectFromPoints(selectVec);
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