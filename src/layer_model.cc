#include "include/layer_model.h"

#include "include/tree_manager.h"
namespace ProjectModel {

LayerModel::LayerModel(TreeManager *psdTreeManager,
                       TreeManager *controllerTreeManger, QObject *parent)
    : QObject(parent) {
  this->controllerTreeManger = controllerTreeManger;
  this->psdTreeManager = psdTreeManager;
  psdTreeManager->setParent(this);
  controllerTreeManger->setParent(this);
}
TreeManager *LayerModel::getPsdTreeManager() const { return psdTreeManager; }
TreeManager *LayerModel::getControllerTreeManger() const {
  return controllerTreeManger;
}
bool BitmapLayer::isContainer() { return false; }

BitmapLayer::BitmapLayer(const QString &name, const LayerBitmap &bitmap,
                         bool isVisible)
    : Layer(name, isVisible), bitmap(bitmap) {}

}  // namespace ProjectModel
