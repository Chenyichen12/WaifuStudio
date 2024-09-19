#include "deformmanager.h"

#include "AbstractDeformer.h"
namespace WaifuL2d {
DeformManager::DeformManager() {
  setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void DeformManager::addDeformer(AbstractDeformer* mopher) {
  deformers.append(mopher);
  mopher->setParentItem(this);
}
void DeformManager::removeDeformer(AbstractDeformer* mopher) {
  // TODO: need to improve this
  deformers.removeOne(mopher);
  mopher->setParentItem(nullptr);
  mopher->setDeformerParent(nullptr);
}
void DeformManager::clearSelection() {
  for (auto& mopher : deformers) {
    mopher->setSelected(false);
  }
}
void DeformManager::selectFromLayers(const QList<Layer*>& layers) {
  for (auto& mopher : deformers) {
    if (layers.contains(mopher->getBindLayer())) {
      mopher->setSelected(true);
    }
  }
}
AbstractDeformer* DeformManager::getDeformerFromLayer(Layer* layer) {
  for (auto& mopher : deformers) {
    if (mopher->getBindLayer() == layer) {
      return mopher;
    }
  }
  return nullptr;
}
QRectF DeformManager::boundingRect() const {
  QRectF total;
  for (auto& deformer : deformers) {
    total = total.united(deformer->boundingRect());
  }

  return total;
}
}  // namespace WaifuL2d