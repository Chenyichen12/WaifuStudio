#include "deformmanager.h"

#include "abstractdeformer.h"
namespace WaifuL2d {
DeformManager::DeformManager() {
  setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void DeformManager::setSmallSize(int size) {
  smallSize = size;
  for (auto& deformer : deformers) {
    deformer->setSmallSize(size);
  }
}

void DeformManager::addDeformer(AbstractDeformer* deformer) {
  deformers.append(deformer);
  deformer->setParentItem(this);
  deformer->setSmallSize(smallSize);
}
void DeformManager::removeDeformer(AbstractDeformer* deformer) {
  // TODO: need to improve this
  deformers.removeOne(deformer);
  deformer->setParentItem(nullptr);
  deformer->setDeformerParent(nullptr);
}
void DeformManager::clearSelection() {
  for (auto& mopher : deformers) {
    mopher->setSelected(false);
  }
}
void DeformManager::selectFromLayers(const QList<Layer*>& layers) {
  for (auto& mopher : deformers) {
    if (layers.contains(mopher->getBindLayer())) {
      mopher->setDeformerSelect(true);
    } else {
      mopher->setDeformerSelect(false);
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

void DeformManager::handleSelectClick(const QPointF& scenePos, bool isMulti) {
  AbstractDeformer* readyDeformer = nullptr;
  for (auto& deformer : deformers) {
    if (deformer->boundingRect().contains(scenePos) &&
        !deformer->isDeformerSelected()) {
      readyDeformer = deformer;
      break;
    }
  }
  if (isMulti) {
    if (readyDeformer) {
      emit deformShouldSelect(getSelectedDeformers() << readyDeformer);
    }
    return;
  }
  if (readyDeformer) {
    emit deformShouldSelect(QList<AbstractDeformer*>() << readyDeformer);
  } else {
    emit deformShouldSelect(QList<AbstractDeformer*>());
  }
}
QList<AbstractDeformer*> DeformManager::getSelectedDeformers() const {
  QList<AbstractDeformer*> selectedDeformers;
  for (auto& deformer : deformers) {
    if (deformer->isDeformerSelected()) {
      selectedDeformers.append(deformer);
    }
  }
  return selectedDeformers;
}
}  // namespace WaifuL2d