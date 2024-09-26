#include "lockcommand.h"

#include "model/tree/layer.h"
#include "model/tree/layermodel.h"

WaifuL2d::LockCommand::LockCommand(LayerModel* model, int layerId,
                                   QUndoCommand* parent): QUndoCommand(parent),
  model(model), layerId(layerId) {
}

void WaifuL2d::LockCommand::redo() {
  auto layer = model->layerFromId(layerId);
  if (layer == nullptr) {
    return;
  }
  layer->setLocked(shouldLock);
}

void WaifuL2d::LockCommand::undo() {
  auto layer = model->layerFromId(layerId);
  if (layer == nullptr) {
    return;
  }
  layer->setLocked(!shouldLock);
}