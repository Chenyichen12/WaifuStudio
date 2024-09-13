#pragma once
#include <QStandardItemModel>
class QItemSelectionModel;
namespace Tree {
class Layer;
class LayerModel : public QStandardItemModel {
  Q_OBJECT
  QItemSelectionModel* selectionModel;
  std::unordered_map<int, Layer*> layerIndex;

 public:
  LayerModel(QObject* parent = nullptr);
  void addLayer(Layer* layer, Layer* parent = nullptr);
  QItemSelectionModel* getSelectionModel() const;
  Layer* findLayerById(int id) const;
  
};
}  // namespace Tree