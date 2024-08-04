#include "layer_model.h"

#include "layer.h"
#include "tree_manager.h"
namespace ProjectModel {

LayerModel::LayerModel(TreeItemModel *psdTreeManager,
                       TreeItemModel *controllerTreeManger, QObject *parent)
    : QObject(parent) {
  this->controllerTreeManger = controllerTreeManger;
  this->psdTreeManager = psdTreeManager;
  psdTreeManager->setParent(this);
  controllerTreeManger->setParent(this);
}
TreeItemModel *LayerModel::getPsdTreeManager() const { return psdTreeManager; }
TreeItemModel *LayerModel::getControllerTreeManger() const {
  return controllerTreeManger;
}

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
