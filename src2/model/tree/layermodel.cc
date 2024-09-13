#include "layermodel.h"

#include <QItemSelectionModel>

#include "layer.h"
namespace Tree {
LayerModel::LayerModel(QObject* parent) : QStandardItemModel(parent) {
  selectionModel = new QItemSelectionModel(this);
}

void LayerModel::addLayer(Layer* layer, Layer* parent) {
  if (parent == nullptr) {
    appendRow(layer);
  } else {
    parent->appendRow(layer);
  }
  this->layerIndex.insert({layer->getId(), layer});
}

QItemSelectionModel* LayerModel::getSelectionModel() const {
  return this->selectionModel;
}

Layer* LayerModel::findLayerById(int id) const {
  if (layerIndex.contains(id) == false) {
    return nullptr;
  }
  return this->layerIndex.at(id);
}

}  // namespace Tree
