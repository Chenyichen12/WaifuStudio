//
// Created by chen_yichen on 2024/7/30.
//

#pragma once
#include <QObject>
#include <vector>

#include "layer.h"
#include "memory"
template <typename T>
class TreeNode {
 protected:
  std::shared_ptr<T> d;
  TreeNode *parent = nullptr;
  std::vector<TreeNode<T> *> children{};

 public:
  TreeNode<T>() = default;
  explicit TreeNode<T>(std::shared_ptr<T> d) { this->d = d; }

  TreeNode<T>(const TreeNode<T> &) = delete;

  const std::vector<TreeNode<T> *> getChildren() const { return children; }
  void setChildren(const std::vector<TreeNode<T>>& children) {
    this->children = children;
  }

  void setData(std::shared_ptr<T> data) { this->d = data; }

  const std::shared_ptr<T> &getData() const { return d; }

  void setParent(TreeNode<T> *node) { this->parent = node; }

  TreeNode *getParent() const { return parent; }

  ~TreeNode() {
    parent = nullptr;
    for (const auto &item : children) {
      delete item;
    }
  }
};

class TreeManager : public QObject {
  Q_OBJECT
 private:
  TreeNode<Layer> *root;

 public:
  explicit TreeManager(QObject *parent = nullptr);

  typedef bool callBack(TreeNode<Layer> *l);

  void forEach(const std::function<callBack> &c) const;
  static void forEach(const TreeNode<Layer> *r,
                      const std::function<callBack> &c);
  TreeNode<Layer> *findNode(const TreeNode<Layer> *source) const;
  TreeNode<Layer> *findNode(const int &id) const;

  TreeNode<Layer> *getRoot() const;
  ~TreeManager() override;
};
