#include "deformercommand.h"

#include <filesystem>

#include "abstractdeformer.h"

namespace WaifuL2d {
DeformerCommand::DeformerCommand() = default;

QUndoCommand* DeformerCommand::createUndoCommand(QUndoCommand* parent) {
  if (info.target == nullptr) {
    throw std::runtime_error("target is nullptr");
  }
  return new BasicMoveCommand(info, parent);
}

DeformerCommand::BasicMoveCommand::BasicMoveCommand(
    WaifuL2d::BasicDeformerCommandInfo info, QUndoCommand* parent)
  : QUndoCommand(parent), info(info) {
}

void DeformerCommand::BasicMoveCommand::redo() {
  info.target->setScenePoints(info.newPoints);
}

void DeformerCommand::BasicMoveCommand::undo() {
  info.target->setScenePoints(info.oldPoints);
}

bool DeformerCommand::BasicMoveCommand::mergeWith(const QUndoCommand* other) {
  if (other->id() != id()) {
    return false;
  }
  auto otherCommand =
      static_cast<const DeformerCommand::BasicMoveCommand*>(other);
  if (otherCommand->info.isStart) {
    return false;
  }

  if (info.target != otherCommand->info.target) {
    return false;
  }
  info.newPoints = otherCommand->info.newPoints;
  info.newPoints = otherCommand->info.newPoints;
  return true;
}
} // namespace WaifuL2d