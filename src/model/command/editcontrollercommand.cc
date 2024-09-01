#include "editcontrollercommand.h"

#include "../scene/editmeshcontroller.h"
namespace Command {
struct EditMeshRecord {
  std::vector<QPointF> pointInfo;
  CDT::EdgeUSet fixedEdge;
  CDT::EdgeUSet allEdge;
  bool isValid() const {
    return !(pointInfo.empty() && fixedEdge.empty() && allEdge.empty());
  }

  EditMeshRecord(const std::vector<QPointF>& point_info,
                 const CDT::EdgeUSet& fixed_edge, const CDT::EdgeUSet& all_edge)
      : pointInfo(point_info), fixedEdge(fixed_edge), allEdge(all_edge) {}
};
EditMeshControllerCommand::EditMeshControllerCommand(
    Scene::EditMeshController* controller, QUndoCommand* parent)
    : QUndoCommand(parent) {
  this->controller = controller;
}

void syncState(const EditMeshRecord& record,
               Scene::EditMeshController* controller) {}

void EditMeshControllerCommand::redo() {
  if (first) {
    first = false;
    return;
  }
  if (!this->canDo()) {
    return;
  }
  controller->setEditMesh(newRecord->pointInfo, newRecord->fixedEdge,
                          newRecord->allEdge);
}
void EditMeshControllerCommand::undo() {
  if (!this->canDo()) {
    return;
  }
  controller->setEditMesh(oldRecord->pointInfo, oldRecord->fixedEdge,
                          oldRecord->allEdge);
}

void EditMeshControllerCommand::recordOldState() {
  this->oldRecord = std::make_unique<EditMeshRecord>(
      controller->getPointFromScene(), controller->getFixedEdge(),
      controller->getAllEdge());
}
void EditMeshControllerCommand::recordNewState() {
  this->newRecord = std::make_unique<EditMeshRecord>(
      controller->getPointFromScene(), controller->getFixedEdge(),
      controller->getAllEdge());
}

EditMeshControllerCommand::~EditMeshControllerCommand() = default;

bool EditMeshControllerCommand::canDo() const {
  return oldRecord != nullptr && newRecord != nullptr;
}
}  // namespace Command