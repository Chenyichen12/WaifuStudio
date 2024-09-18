#include "deformmanager.h"

#include "AbstractDeformer.h"
namespace WaifuL2d {
DeformManager::DeformManager() {
  setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void DeformManager::addDeformer(AbstractDeformer* mopher) {
  deformer.append(mopher);
  mopher->setParentItem(this);
}
void DeformManager::removeDeformer(AbstractDeformer* mopher) {
  // TODO: need to improve this
  deformer.removeOne(mopher);
  mopher->setParentItem(nullptr);
  mopher->setMopherParent(nullptr);
}
void DeformManager::clearSelection() {
  for (auto& mopher : deformer) {
    mopher->setSelected(false);
  }
}
void DeformManager::selectFromLayers(const QList<Layer*>& layers) {
  for (auto& mopher : deformer) {
    if (layers.contains(mopher->getBindLayer())) {
      mopher->setSelected(true);
    }
  }
}
}  // namespace WaifuL2d