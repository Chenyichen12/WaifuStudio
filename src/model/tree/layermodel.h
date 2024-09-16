#pragma once
#include <QStandardItemModel>
class QItemSelectionModel;
namespace WaifuL2d {
class Layer;
class LayerModel : public QStandardItemModel {
  Q_OBJECT
 public:
  LayerModel(QObject* parent = nullptr);
  void addLayer(Layer* layer, Layer* parent = nullptr);
  QItemSelectionModel* getSelectionModel() const;
  void removeLayer(Layer* layer, Layer* parent = nullptr);

  Layer* layerFromIndex(const QModelIndex& index) const;
};
}  // namespace WaifuL2d