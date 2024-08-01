//
// Created by chen_yichen on 2024/7/30.
//

#include <utility>
#include "model/tree_manager.h"
namespace ProjectModel{
TreeManager::TreeManager(QObject *parent) : QObject(parent) {
  root = new TreeNode<Layer>();
}
void TreeManager::forEach(const std::function<callBack> &c) const {
  TreeManager::forEach(root, c);
}
void TreeManager::forEach(const TreeNode<Layer> *r,
                          const std::function<callBack> &c) {
  for (TreeNode<Layer> *item : r->getChildren()) {
    bool res = c(item);
    if (!res)
      return;
    TreeManager::forEach(item, c);
  }
}
TreeNode<Layer> *TreeManager::findNode(const TreeNode<Layer> *source) const {
  TreeNode<Layer> *resPtr = nullptr;
  this->forEach([&](TreeNode<Layer> *l) {
    if (l == source) {
      resPtr = l;
      return false;
    }
    return true;
  });
  return resPtr;
}
TreeNode<Layer> *TreeManager::findNode(const int &id) const {
  TreeNode<Layer> *resPtr = nullptr;
  this->forEach([&](TreeNode<Layer> *l) {
    if (l->getData()->getId() == id) {
      resPtr = l;
      return false;
    }
    return true;
  });
  return resPtr;
}
TreeNode<Layer> *TreeManager::getRoot() const { return root; }
TreeManager::~TreeManager() {
  delete this->root;
}

}
