#pragma once
#include <QStandardItemModel>
#include <QItemSelection>
class QItemSelectionModel;
namespace Tree {
class Layer;

class LayerModel : public QStandardItemModel {
  Q_OBJECT
 private:
  QItemSelectionModel* selectionModel;
protected:
virtual void handleSelectionModelChanged(const QItemSelection &selected, const QItemSelection &deselected);
 public:
  LayerModel(QObject* parent = nullptr);
  Layer* findLayerById(int id) const;
  void selectLayer(const QList<int>& id);

  QItemSelectionModel* getSelectionModel() const { return selectionModel; }
signals:
void LayerSelectionChanged(const QList<int> id);
 public slots:
  virtual void handleToggleVisible(QModelIndex index);
  virtual void recordToggleVisble(QModelIndexList listIndex);
};
}  // namespace Tree