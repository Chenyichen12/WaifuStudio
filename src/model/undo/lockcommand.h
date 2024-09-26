#pragma once

#include <QUndoCommand>
#include "waifucommand.h"

namespace WaifuL2d {
class LayerModel;

class LockCommand : public QUndoCommand {
private:
  LayerModel* model;
  int layerId;
  bool shouldLock = true;

public:
  LockCommand(LayerModel* model, int layerId, QUndoCommand* parent = nullptr);
  void setShouldLock(bool shouldLock) { this->shouldLock = shouldLock; }

  void redo() override;
  void undo() override;
  int id() const override { return CommandId::LockCommandId; }
};
}