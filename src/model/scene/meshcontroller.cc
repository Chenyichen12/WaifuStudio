#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include "mesh.h"
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
class MeshController::MeshControllerEventHandler {
 private:
  MeshController* controller;
  int currentPressedIndex = -1;

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
    controller->setMeshPointScene(currentPressedIndex, pos);
    controller->upDateMeshBuffer();
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


void MeshController::unSelectTheController() {
  AbstractController::unSelectTheController();
  this->unSelectPoint();
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
  this->selectRectItem = new RectSelectController(this);

  selectRectItem->moveCallBack = [this](const auto& p1, const auto& p2) {
    auto delta = p2 - p1;
    for (int i = 0; i < selectedPoint.size(); ++i) {
      if (selectedPoint[i]) {
        auto point = this->controlMesh->getVertices()[i].pos;
        point.x += delta.x();
        point.y += delta.y();
        this->setMeshPointScene(i,QPointF(point.x,point.y));
      }
    }
    this->upDateMeshBuffer();
  };

  auto r = findRootController(this);
  if (r != nullptr) {
    selectRectItem->setPadding(r->boundingRect().width() / 100);
    selectRectItem->setLineWidth(r->boundingRect().width() / 500);
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
  selectRectItem->setBoundRect({});
  this->update();
}

void MeshController::selectPoint(int index) {
  if (index < 0 || index >= selectedPoint.size()) {
    return;
  }
  this->selectedPoint[index] = true;
  const auto& vec = getSelectedPointScenePosition();
  const auto& rect = RectSelectController::boundRectFromPoints(vec);
  selectRectItem->setBoundRect(rect);
  this->update();
}

void MeshController::upDateMeshBuffer() const {
  this->controlMesh->upDateBuffer();
}

void MeshController::setMeshPointScene(int index,
                                       const QPointF& scenePosition) {
  auto vec = controlMesh->getVertices()[index];
  vec.pos.x = scenePosition.x();
  vec.pos.y = scenePosition.y();
  controlMesh->setVerticesAt(index, vec);
  this->update();
}

void MeshController::setMeshPointFromLocal(int index,
                                           const QPointF& localPosition) {
  auto scenePos = localPointToScene(localPosition);
  setMeshPointScene(index, scenePos);
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