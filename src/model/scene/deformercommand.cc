#include "deformercommand.h"

#include "abstractdeformer.h"
namespace WaifuL2d {
DeformerCommand::DeformerCommand(AbstractDeformer* mopher)
    : targetMopher(mopher) {}

DeformerPointCommand::DeformerPointCommand(WaifuL2d::AbstractDeformer* deformer,
                                           bool isEnd)
    : DeformerCommand(deformer), isEnd(isEnd) {}
QUndoCommand* DeformerPointCommand::createUndoCommand(QUndoCommand* parent) {
  return new PointUndoCommand(this->targetMopher, this->oldPoints,
                              this->newPoints, this->isEnd, parent);
}
DeformerPointCommand::PointUndoCommand::PointUndoCommand(
    AbstractDeformer* deformer, const QList<QPointF>& oldPoints,
    const QList<QPointF>& newPoints, bool isEnd, QUndoCommand* parent)
    : QUndoCommand(parent),
      deformer(deformer),
      oldPoints(oldPoints),
      newPoints(newPoints),
      isEnd(isEnd) {}

void DeformerPointCommand::PointUndoCommand::undo() {
  deformer->setScenePoints(oldPoints);
}
void DeformerPointCommand::PointUndoCommand::redo() {
  deformer->setScenePoints(newPoints);
}

int DeformerPointCommand::PointUndoCommand::id() const {
  return DeformerCommand::PointCommand;
}

bool DeformerPointCommand::PointUndoCommand::mergeWith(
    const QUndoCommand* other) {
  if (other->id() != id()) return false;
  auto otherCommand =
      static_cast<const DeformerPointCommand::PointUndoCommand*>(other);
  if (this->isEnd) {
    return false;
  }
  if (deformer != otherCommand->deformer) {
    return false;
  }
  newPoints = otherCommand->newPoints;
  return true;
}
}  // namespace WaifuL2d
