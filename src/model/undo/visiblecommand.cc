#include "visiblecommand.h"

#include "model/tree/layer.h"
#include "model/tree/layermodel.h"
namespace WaifuL2d {
VisibleCommand::VisibleCommand(RenderGroup* renderGroup, LayerModel* layerModel,
                               DeformManager* deformManager,
                               const QModelIndex& index, bool visible)
    : renderGroup(renderGroup),
      layerModel(layerModel),
      deformManager(deformManager),
      indexes({index}),
      visibles({visible}) {}

void VisibleCommand::setStart(bool start) { this->isStart = start; }

void VisibleCommand::undo() {
  for (int i = 0; i < indexes.size(); i++) {
    auto layer = layerModel->layerFromIndex(indexes[i]);
    if (layer == nullptr) {
      continue;
    }
    if (layer->getLocked()) {
      continue;
    }
    layer->setVisible(!visibles[i]);
  }
}

void VisibleCommand::redo() {
  qDebug() << indexes.size();
  for (int i = 0; i < indexes.size(); i++) {
    auto layer = layerModel->layerFromIndex(indexes[i]);
    if (layer == nullptr) {
      continue;
    }
    if (layer->getLocked()) {
      continue;
    }
    layer->setVisible(visibles[i]);
  }
}

bool VisibleCommand::mergeWith(const QUndoCommand* command) {
  if (command->id() != id()) {
    return false;
  }
  auto other = static_cast<const VisibleCommand*>(command);
  if (other->isStart) {
    return false;
  }
  if (other->deformManager != deformManager ||
      other->renderGroup != renderGroup || other->layerModel != layerModel) {
    return false;
  }

  for (int i = 0; i < other->indexes.size(); i++) {
    indexes.append(other->indexes[i]);
    visibles.append(other->visibles[i]);
  }
  return true;
}

}  // namespace WaifuL2d
