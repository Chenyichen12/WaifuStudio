#include "deformercommand.h"

#include "abstractdeformer.h"
namespace WaifuL2d {
DeformerCommand::DeformerCommand(AbstractDeformer* deformer,
                                 QUndoCommand* parent)
    : QUndoCommand(parent), targetDeformer(deformer) {}
DeformerCommand::DeformerCommand(const DeformerCommand& other,
                                 QUndoCommand* parent)
    : QUndoCommand(parent) {
  this->targetDeformer = other.targetDeformer;
  this->isEnd = other.isEnd;
  this->oldPoints = other.oldPoints;
  this->newPoints = other.newPoints;
  this->oldAngle = other.oldAngle;
  this->newAngle = other.newAngle;
}

void DeformerCommand::redo() { targetDeformer->setScenePoints(newPoints); }

void DeformerCommand::undo() { targetDeformer->setScenePoints(oldPoints); }

bool DeformerCommand::mergeWith(const QUndoCommand* other) {
  if (other->id() != id()) {
    return false;
  }
  if(this->isEnd){
    return false;
  }
  auto otherCommand = static_cast<const DeformerCommand*>(other);
  if (targetDeformer != otherCommand->targetDeformer) {
    return false;
  }
  this->isEnd = otherCommand->isEnd;
  newPoints = otherCommand->newPoints;
  newAngle = otherCommand->newAngle;
  return true;
}
}  // namespace WaifuL2d
