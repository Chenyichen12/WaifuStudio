//
// Created by chen_yichen on 2024/7/30.
//

#pragma once

#include <QStandardItemModel>

namespace ProjectModel {

class TreeItemModel : public QStandardItemModel {
 public:
  explicit TreeItemModel(QObject *parent = nullptr);
  typedef bool callBack(QStandardItem *l);

  void forEach(const std::function<callBack> &c) const;

  static void forEach(const QStandardItem *r, const std::function<callBack> &c);

  QStandardItem *findNode(const int &id) const;

  static std::vector<QStandardItem *> itemChild(const QStandardItem* item);
  ~TreeItemModel() override;
};

}  // namespace ProjectModel
