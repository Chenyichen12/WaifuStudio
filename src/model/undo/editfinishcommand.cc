#include "editfinishcommand.h"

#include <execution>

#include "model/scene/deformer/meshdeformer.h"
#include "model/scene/mesh/mesh.h"
namespace WaifuL2d {
struct EditFinishCommand::Data {
  QList<MeshVertex> newPoints;
  QList<unsigned int> newIncidents;

  QList<MeshVertex> oldPoints;
  QList<unsigned int> oldIncidents;

};

EditFinishCommand::EditFinishCommand(MainStageScene* scene,
                                     MeshDeformer* deformer,
                                     const QList<QPointF>& changePoints,
                                     const QList<unsigned int>& incident,
                                     QUndoCommand* parent)
  : QUndoCommand(parent), scene(scene), deformer(deformer) {
  auto mesh = deformer->getMesh();
  d = std::make_unique<Data>();
  d->newIncidents = incident;
  for (const auto& p : changePoints) {
    auto uv = mesh->uvAtPoint(p);
    d->newPoints.append({glm::vec2(p.x(), p.y()), glm::vec2(uv.x(), uv.y())});
  }

  d->oldPoints = mesh->getVertices();
  d->oldIncidents = mesh->getIncident();
}

EditFinishCommand::~EditFinishCommand() = default;

void EditFinishCommand::redo() {
  deformer->getMesh()->setMeshStruct(d->newPoints, d->newIncidents);
  deformer->fitMesh();
}

void EditFinishCommand::undo() {
  deformer->getMesh()->setMeshStruct(d->oldPoints, d->oldIncidents);
  deformer->fitMesh();
}

EditFinishCommand*
EditFinishCommandWrapper::createCommand(QUndoCommand* parent) const {
  Q_ASSERT(scene_ != nullptr);
  Q_ASSERT(deformer_ != nullptr);
  return new EditFinishCommand(scene_, deformer_, changePoints, incident,
                               parent);

}

void EditFinishCommandWrapper::setTarget(MainStageScene* scene,
                                         MeshDeformer* deformer) {
  scene_ = scene;
  deformer_ = deformer;
}

void EditFinishCommandWrapper::setData(const QList<QPointF>& changePoints,
                                       const QList<unsigned int>& incident) {
  this->changePoints = changePoints;
  this->incident = incident;
}
}