#include "layermodel.h"

#include <QItemSelectionModel>
#include <stack>

#include "layer.h"
namespace Tree {
LayerModel::LayerModel(QObject* parent) : QStandardItemModel(parent) {
  this->selectionModel = new QItemSelectionModel(this);
  connect(this->selectionModel, &QItemSelectionModel::selectionChanged, this,
          &LayerModel::handleSelectionModelChanged);
}
void LayerModel::recordToggleVisble(QModelIndexList listIndex) {}
void LayerModel::handleToggleVisible(QModelIndex index) {
  if (!index.isValid()) {
    return;
  }
  auto item = this->itemFromIndex(index);
  auto visible = item->data(VisibleRole).toBool();
  item->setData(!visible);
}

void LayerModel::selectLayer(const QList<int>& id) {
  auto indexes = QModelIndexList();
  for (int i : id) {
    auto layer = this->findLayerById(i);
    if (layer) {
      indexes.append(layer->index());
    }
  }
  disconnect(this->selectionModel, &QItemSelectionModel::selectionChanged, this,
             &LayerModel::handleSelectionModelChanged);
  this->selectionModel->clear();
  for (const auto& index : indexes) {
    this->selectionModel->select(index, QItemSelectionModel::Select);
  }
  connect(this->selectionModel, &QItemSelectionModel::selectionChanged, this,
          &LayerModel::handleSelectionModelChanged);
}

Layer* LayerModel::findLayerById(int id) const {
  // recursive
  std::stack<QStandardItem*> parseStack;
  parseStack.push(this->invisibleRootItem());
  while (!parseStack.empty()) {
    auto item = parseStack.top();
    parseStack.pop();
    if (item->data(IdRole).toInt() == id) {
      return static_cast<Layer*>(item);
    }
    for (int i = 0; i < item->rowCount(); i++) {
      parseStack.push(item->child(i));
    }
  }
  return nullptr;
}
void LayerModel::handleSelectionModelChanged(const QItemSelection& selected,
                                             const QItemSelection& deselected) {
  Q_UNUSED(deselected);
  Q_UNUSED(selected);

  auto indexes = selectionModel->selectedIndexes();
  QList<int> selectedIds;
  for (const auto& index : indexes) {
    if (index.isValid()) {
      selectedIds.append(index.data(IdRole).toInt());
    }
  }
  emit LayerSelectionChanged(selectedIds);
}

}  // namespace Tree
