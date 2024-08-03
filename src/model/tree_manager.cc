//
// Created by chen_yichen on 2024/7/30.
//

#include "tree_manager.h"

#include "layer.h"

namespace ProjectModel {

// TreeItemModel::TreeItemModel(QObject *parent) : QStandardItemModel(parent) {}

TreeItemModel::TreeItemModel(QObject *parent) : QStandardItemModel(parent) {}

void TreeItemModel::forEach(const std::function<callBack> &c) const {
  TreeItemModel::forEach(this->invisibleRootItem(), c);
}
void TreeItemModel::forEach(const QStandardItem *r,
                            const std::function<callBack> &c) {
  for (int i = 0; i < r->rowCount(); ++i) {
    auto item = r->child(i);
    bool res = c(item);
    if (!res) {
      return;
    }
    TreeItemModel::forEach(item, c);
  }
}
QStandardItem *TreeItemModel::findNode(const int &id) const {
  QStandardItem *resPtr = nullptr;
  this->forEach([&](QStandardItem *l) {
    if (l->data(UserIdRole) == id) {
      resPtr = l;
      return false;
    }
    return true;
  });
  return resPtr;
}

std::vector<QStandardItem *> TreeItemModel::itemChild(
    const QStandardItem *item) {
  auto res = std::vector<QStandardItem *>();
  for (int i = 0; i < item->rowCount(); ++i) {
    res.push_back(item->child(i));
  }
  return res;
}

Qt::ItemFlags TreeItemModel::flags(const QModelIndex& index) const {
  if (!index.isValid()) {
    return QStandardItemModel::flags(index);
  }
  auto itemCanDrop =
      this->itemFromIndex(index)->type() == LayerTypes::PsGroupLayerType;
  auto flags = QStandardItemModel::flags(index);

  if (itemCanDrop) {
    flags.setFlag(Qt::ItemIsDropEnabled, true);
  } else {
    flags.setFlag(Qt::ItemIsDropEnabled, false);
  }

  return flags;
}

TreeItemModel::~TreeItemModel() = default;
}  // namespace ProjectModel
