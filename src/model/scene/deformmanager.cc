#include "deformmanager.h"

#include "abstractdeformer.h"
namespace WaifuL2d {
DeformManager::DeformManager() {
  setFlag(QGraphicsItem::ItemIsSelectable, false);
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
AbstractDeformer* DeformManager::getDeformerFromLayer(
    const Layer* layer) const {
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
void DeformManager::emitDeformCommand(QSharedPointer<MopherCommand> command) {
  emit deformCommand(command);
}

//void DeformManager::handleSelectClick(const QPointF& scenePos, bool isMulti,
//                                      bool& isChanged) {
//  AbstractDeformer* readyDeformer = nullptr;
//  for (auto& deformer : deformers) {
//    if (deformer->boundingRect().contains(scenePos) &&
//        !deformer->isSelected()) {
//      readyDeformer = deformer;
//      break;
//    }
//  }
//  if (isMulti) {
//    if (readyDeformer) {
//      readyDeformer->setSelected(true);
//      isChanged = true;
//    } else {
//      isChanged = false;
//    }
//    return;
//  }
//
//  for (auto& deformer : deformers) {
//    if (deformer->isSelected() && deformer != readyDeformer) {
//      deformer->setSelected(false);
//      isChanged = true;
//    }
//  }
//}
}  // namespace WaifuL2d