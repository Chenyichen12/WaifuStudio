#pragma once
#include <QStandardItemModel>
class QItemSelectionModel;
namespace WaifuL2d {
class Layer;
class LayerModel : public QStandardItemModel {
  Q_OBJECT
  int AutoIncrementId = 0;
  std::unordered_map<int, Layer*> quickFindCache;

 public:
  LayerModel(QObject* parent = nullptr);
  void addLayer(Layer* layer, Layer* parent = nullptr);
  void removeLayer(Layer* layer, Layer* parent = nullptr);

  Layer* layerFromIndex(const QModelIndex& index) const;
  Layer* layerFromId(int id) const;
  QList<Layer*> getOrderedList() const;

  int getAutoIncrementId() const { return AutoIncrementId; }
};
}  // namespace WaifuL2d