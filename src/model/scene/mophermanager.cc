#include "mophermanager.h"

#include "abstractmopher.h"
namespace WaifuL2d {
MopherManager::MopherManager() {
  setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void MopherManager::addMopher(AbstractMopher* mopher) {
  mophers.append(mopher);
  mopher->setParentItem(this);
}
void MopherManager::removeMopher(AbstractMopher* mopher) {
  // TODO: need to improve this
  mophers.removeOne(mopher);
  mopher->setParentItem(nullptr);
  mopher->setMopherParent(nullptr);
}
void MopherManager::clearSelection() {
  for (auto& mopher : mophers) {
    mopher->setSelected(false);
  }
}
void MopherManager::selectFromLayers(const QList<Layer*>& layers) {
  for (auto& mopher : mophers) {
    if (layers.contains(mopher->getBindLayer())) {
      mopher->setSelected(true);
    }
  }
}
}  // namespace WaifuL2d