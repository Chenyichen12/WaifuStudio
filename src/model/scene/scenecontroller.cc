#include "scenecontroller.h"

#include <QPainter>

#include "mesh.h"
namespace Scene {
MeshController::MeshController(Mesh* controlMesh, QGraphicsItem* parent)
    : AbstractController(parent) {
  this->controlMesh = controlMesh;
}

QRectF MeshController::boundingRect() const {
  return controlMesh->boundingRect();
}

void MeshController::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {
  const auto& meshPoint = controlMesh->getVertices();
  auto scale = painter->transform().m11();

  auto pen = QPen(Qt::black);
  pen.setWidth(1/scale);
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


  for (const auto& mesh_point : meshPoint) {
    painter->drawEllipse(QPointF(mesh_point.pos.x, mesh_point.pos.y), 3 / scale,
                         3 / scale);
  }
}

int MeshController::controllerId() { return controlMesh->getLayerId(); }

int MeshController::type() const { return ControllerType::MeshControllerType; }
}  // namespace Scene
