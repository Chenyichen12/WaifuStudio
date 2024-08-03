#include "layer_model.h"

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
}  // namespace ProjectModel
