#include "editmeshcontroller.h"

#include <QPainter>

#include "pointeventhandler.h"
#include "scenecontroller.h"
namespace Scene {

class EditMeshPointHandler : public PointEventHandler {
 private:
  EditMeshController* controller;

 protected:
  void pointMoveEvent(int current, QGraphicsSceneMouseEvent* event) override;
  void pointPressedEvent(int index, QGraphicsSceneMouseEvent* event) override;
  void pointReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 public:
  explicit EditMeshPointHandler(EditMeshController* controller)
      : PointEventHandler(controller) {
    this->controller = controller;
  }
};

EditMeshController::EditMeshController(const std::vector<MeshVertex>& vertices,
                                       const std::vector<int>& incident,
                                       QGraphicsItem* parent)
    : AbstractController(parent), vertices(vertices), incident(incident) {
  this->pointHandler = new EditMeshPointHandler(this);
}

EditMeshController::~EditMeshController() { delete this->pointHandler; }

void EditMeshController::paint(QPainter* painter,
                               const QStyleOptionGraphicsItem* option,
                               QWidget* widget) {
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
    // painter->drawEllipse(QPointF(point.pos.x, point.pos.y), 3 / scale,
    // handler->AbsolutePointRadius / scale);
  }
}

QRectF EditMeshController::boundingRect() const { return {}; }

int EditMeshController::type() const {
  return ControllerType::EditMeshControllerType;
}

QPointF EditMeshController::localPointToScene(const QPointF& point) {
  return controllerParent->localPointToScene(point);
}

QPointF EditMeshController::scenePointToLocal(const QPointF& point) {
  return controllerParent->scenePointToLocal(point);
}
}  // namespace Scene
