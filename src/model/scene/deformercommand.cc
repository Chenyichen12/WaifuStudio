#include "deformercommand.h"

#include "abstractdeformer.h"
namespace WaifuL2d {
DeformerCommand::DeformerCommand() = default;
QUndoCommand *DeformerCommand::createUndoCommand(QUndoCommand *parent) {
  if (info.target == nullptr) {
    throw std::runtime_error("target is nullptr");
  }
  return new BasicMoveCommand(info, parent);
}

DeformerCommand::BasicMoveCommand::BasicMoveCommand(
    WaifuL2d::BasicDeformerCommandInfo info, QUndoCommand *parent)
    : QUndoCommand(parent), info(info) {}
void DeformerCommand::BasicMoveCommand::redo() {
  info.target->setScenePoints(info.newPoints);
}

void DeformerCommand::BasicMoveCommand::undo() {
  info.target->setScenePoints(info.oldPoints);
}

bool DeformerCommand::BasicMoveCommand::mergeWith(const QUndoCommand *other) {
  if (other->id() != id()) {
    return false;
  }
  if (this->info.isEnd) {
    return false;
  }
  auto otherCommand =
      static_cast<const DeformerCommand::BasicMoveCommand *>(other);
  if (info.target != otherCommand->info.target) {
    return false;
  }
  info.isEnd = otherCommand->info.isEnd;
  info.newPoints = otherCommand->info.newPoints;
  info.newPoints = otherCommand->info.newPoints;
  return true;
}

// DeformerCommand::DeformerCommand(AbstractDeformer* deformer,
//                                  QUndoCommand* parent)
//     : QUndoCommand(parent), targetDeformer(deformer) {}
// DeformerCommand::DeformerCommand(const DeformerCommand& other,
//                                  QUndoCommand* parent)
//     : QUndoCommand(parent) {
//   this->targetDeformer = other.targetDeformer;
//   this->isEnd = other.isEnd;
//   this->oldPoints = other.oldPoints;
//   this->newPoints = other.newPoints;
//   this->oldAngle = other.oldAngle;
//   this->newAngle = other.newAngle;
// }
//
// void DeformerCommand::redo() { targetDeformer->setScenePoints(newPoints); }
//
// void DeformerCommand::undo() { targetDeformer->setScenePoints(oldPoints); }
//
// bool DeformerCommand::mergeWith(const QUndoCommand* other) {
//   if (other->id() != id()) {
//     return false;
//   }
//   if(this->isEnd){
//     return false;
//   }
//   auto otherCommand = static_cast<const DeformerCommand*>(other);
//   if (targetDeformer != otherCommand->targetDeformer) {
//     return false;
//   }
//   this->isEnd = otherCommand->isEnd;
//   newPoints = otherCommand->newPoints;
//   newAngle = otherCommand->newAngle;
//   return true;
// }

}  // namespace WaifuL2d
