#pragma once
#include <QObject>

namespace ProjectModel {
class TreeItemModel;
class LayerModel : public QObject {
  Q_OBJECT
 private:
  TreeItemModel* psdTreeManager;
  TreeItemModel* controllerTreeManger;

 public:
  LayerModel(TreeItemModel* psdTreeManager, TreeItemModel* controllerTreeManger,
             QObject* parent = nullptr);

  TreeItemModel* getPsdTreeManager() const;
  TreeItemModel* getControllerTreeManger() const;

 public slots:
  void setItemVisible(const QModelIndex& index, bool visible);
};
}  // namespace ProjectModel
