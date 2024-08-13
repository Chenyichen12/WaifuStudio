#include "layer_model.h"

#include <QItemSelectionModel>

#include "layer.h"
#include "tree_manager.h"
namespace ProjectModel {
void LayerModel::handleSelectionChanged(const QItemSelection &selected,
                                        const QItemSelection &deselected) {
  auto which = static_cast<QItemSelectionModel *>(sender());
  auto idList = std::vector<int>();
  for (const auto &selected_index : which->selectedIndexes()) {
    idList.emplace_back(selected_index.data(UserIdRole).toInt());
  }
  emit selectionChanged(idList);
}

LayerModel::LayerModel(TreeItemModel *psdTreeManager,
                       TreeItemModel *controllerTreeManger, QObject *parent)
    : QObject(parent) {
  this->controllerTreeManger = controllerTreeManger;
  this->psdTreeManager = psdTreeManager;
  psdTreeManager->setParent(this);
  controllerTreeManger->setParent(this);

  this->psdTreeSelectionModel = new QItemSelectionModel(psdTreeManager);
  this->controllerTreeSelectionModel =
      new QItemSelectionModel(controllerTreeManger);
  connect(psdTreeSelectionModel, &QItemSelectionModel::selectionChanged, this,
          &LayerModel::handleSelectionChanged, Qt::DirectConnection);
  connect(controllerTreeSelectionModel, &QItemSelectionModel::selectionChanged,
          this, &LayerModel::handleSelectionChanged, Qt::DirectConnection);
}
TreeItemModel *LayerModel::getPsdTreeManager() const { return psdTreeManager; }
TreeItemModel *LayerModel::getControllerTreeManger() const {
  return controllerTreeManger;
}

QItemSelectionModel *LayerModel::getPsdTreeSelectionModel() const {
  return psdTreeSelectionModel;
}

QItemSelectionModel *LayerModel::getControllerTreeSelectionModel() const {
  return controllerTreeSelectionModel;
}

void LayerModel::selectItems(const std::vector<int> &selectionId) {
  // we need to do this to defence loop
  blockSignals(true);
  auto treeSelection = QItemSelection();
  auto controllerSelection = QItemSelection();
  for (int selection_id : selectionId) {
    auto treeNode = psdTreeManager->findNode(selection_id);
    if (treeNode != nullptr) {
      treeSelection.select(treeNode->index(), treeNode->index());
    }
    auto controllerNode = controllerTreeManger->findNode(selection_id);
    if (controllerNode != nullptr) {
      controllerSelection.select(controllerNode->index(),
                                 controllerNode->index());
    }
  }
  psdTreeSelectionModel->select(treeSelection,
                                QItemSelectionModel::ClearAndSelect);
  controllerTreeSelectionModel->select(controllerSelection,
                                       QItemSelectionModel::ClearAndSelect);

  blockSignals(false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LayerModel::setItemVisible(const QModelIndex &index, bool visible) {
  if (index.model() == this->psdTreeManager) {
    psdTreeManager->setData(index, visible, VisibleRole);
    auto n = controllerTreeManger->findNode(index.data(UserIdRole).toInt());
    if (n == nullptr) {
      return;
    }
    n->setData(visible, VisibleRole);
  }

  if (index.model() == this->controllerTreeManger) {
    controllerTreeManger->setData(index, visible, VisibleRole);
    auto n = psdTreeManager->findNode(index.data(UserIdRole).toInt());
    if (n == nullptr) {
      return;
    }
    n->setData(visible, VisibleRole);
  }
}
}  // namespace ProjectModel
