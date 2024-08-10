#include "scenecontroller.h"

#include <QPainter>

#include "mesh.h"
namespace Scene {
RootController::RootController(int width, int height) {
  this->width = width;
  this->height = height;
}

QRectF RootController::boundingRect() const {
  return QRectF(0, 0, width, height);
}

MeshController::MeshController(Mesh* controlMesh, AbstractController* parent)
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

  for (const auto& mesh_point : meshPoint) {
    painter->drawEllipse(QPointF(mesh_point.pos.x, mesh_point.pos.y), 3 / scale,
                         3 / scale);
  }
}

int MeshController::controllerId() { return controlMesh->getLayerId(); }

int MeshController::type() const { return ControllerType::MeshControllerType; }

QPointF MeshController::localPointToScene(const QPointF& point) {
  auto par = static_cast<AbstractController*>(this->parentItem());
  return par->localPointToScene(point);
}

QPointF MeshController::scenePointToLocal(const QPointF& point) {
  auto par = static_cast<AbstractController*>(this->parentItem());
  return par->scenePointToLocal(point);
}

void RootController::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {}

int RootController::controllerId() { return -1; }

int RootController::type() const { return ControllerType::RootControllerType; }

QPointF RootController::localPointToScene(const QPointF& point) {
  auto x = point.x() * this->width;
  auto y = point.y() * this->height;
  return {x, y};
}

QPointF RootController::scenePointToLocal(const QPointF& point) {
  auto x = point.x() / this->width;
  auto y = point.y() / this->height;
  return {x, y};
}
}  // namespace Scene
