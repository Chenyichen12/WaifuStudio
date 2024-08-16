#include "controllercommand.h"

#include "../scene/scenecontroller.h"
namespace Command {
ControllerCommand::ControllerCommand(
    Scene::AbstractController* controller,QUndoCommand* parent)
    : QUndoCommand(parent) {
  this->controller = controller;
}

void ControllerCommand::addOldInfo(const ControllerCommandInfo& info) {
  oldVertex.append(info);
}

void ControllerCommand::addNewInfo(const ControllerCommandInfo& info) {
  newVertex.append(info);
}

void ControllerCommand::undo() {
  for (const auto& old_vertex : oldVertex) {
    controller->setPointFromScene(old_vertex.index, old_vertex.p);
  }
}

void ControllerCommand::redo() {
  for (const auto& new_vertex : newVertex) {
    controller->setPointFromScene(new_vertex.index, new_vertex.p);
  }
}

MeshControllerCommand::MeshControllerCommand(
    Scene::MeshController* controller, QUndoCommand* parent)
    : ControllerCommand(controller, parent) {}

void MeshControllerCommand::undo() {
  ControllerCommand::undo();
  auto* con = static_cast<Scene::MeshController*>(controller);
  con->upDateMeshBuffer();
}

void MeshControllerCommand::redo() {
  if (first) {
    first = false;
    return;
  }
  ControllerCommand::redo();
  auto* con = static_cast<Scene::MeshController*>(controller);
  con->upDateMeshBuffer();
}

}  // namespace Command
