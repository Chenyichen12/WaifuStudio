#include "layermodel.h"

#include <QItemSelectionModel>
#include <QStack>

#include "layer.h"
namespace WaifuL2d {

LayerModel::LayerModel(QObject* parent) : QStandardItemModel(parent) {}

void LayerModel::addLayer(Layer* layer, Layer* parent) {
  if (parent == nullptr) {
    insertRow(0, layer);
  } else {
    parent->insertRow(0, layer);
  }
  layer->setId(AutoIncrementId++);
  quickFindCache.insert(std::make_pair(layer->getId(), layer));
}

Layer* LayerModel::layerFromId(int id) const {
  auto it = quickFindCache.find(id);
  if (it != quickFindCache.end()) {
    return it->second;
  }
  return nullptr;
}

void LayerModel::removeLayer(Layer* layer, Layer* parent) {
  if (parent == nullptr) {
    removeRow(layer->row());
  } else {
    parent->removeRow(layer->row());
  }
  quickFindCache.erase(layer->getId());
}

Layer* LayerModel::layerFromIndex(const QModelIndex& index) const {
  if (!index.isValid()) {
    return nullptr;
  }
  return static_cast<Layer*>(itemFromIndex(index));
}

QList<Layer*> LayerModel::getOrderedList() const {
  QList<Layer*> list;
  QStack<QStandardItem*> parseStack;
  parseStack.push(invisibleRootItem());
  while (!parseStack.isEmpty()) {
    QStandardItem* item = parseStack.pop();
    for (int i = 0; i < item->rowCount(); i++) {
      parseStack.push(item->child(i));
    }
    Layer* layer = static_cast<Layer*>(item);
    list.append(layer);
  }
  return list;
}
}  // namespace WaifuL2d
