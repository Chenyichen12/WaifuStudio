#pragma once

#include <qstandarditemmodel.h>
#include "model/tree_manager.h"
namespace View {

class TreeItem: public QStandardItem {
private:
  int id;
public:
  TreeItem(int id);

  int getId() const;
};

class TreeModel : public QStandardItemModel {
 public:
  static std::unique_ptr<TreeModel> fromManager(const ProjectModel::TreeManager* manager);

  explicit TreeModel(QObject* parent = nullptr);
};
}  // namespace View
