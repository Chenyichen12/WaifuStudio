#include "layermodel.h"

#include <QItemSelectionModel>

#include "layer.h"
namespace WaifuL2d {

LayerModel::LayerModel(QObject* parent) : QStandardItemModel(parent) {}

void LayerModel::addLayer(Layer* layer, Layer* parent) {
  if (parent == nullptr) {
    appendRow(layer);
  } else {
    parent->appendRow(layer);
  }
}

void LayerModel::removeLayer(Layer* layer, Layer* parent) {
  if (parent == nullptr) {
    removeRow(layer->row());
  } else {
    parent->removeRow(layer->row());
  }
}

}  // namespace WaifuL2d
