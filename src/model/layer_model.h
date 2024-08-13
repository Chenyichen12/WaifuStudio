#pragma once
#include <QObject>

class QItemSelectionModel;
class QItemSelection;
namespace ProjectModel {
class TreeItemModel;

class LayerModel : public QObject {
  Q_OBJECT
 private:
  TreeItemModel* psdTreeManager;
  QItemSelectionModel* psdTreeSelectionModel;
  TreeItemModel* controllerTreeManger;
  QItemSelectionModel* controllerTreeSelectionModel;

  void handleSelectionChanged(const QItemSelection& selected,
                              const QItemSelection& deselected);

 public:
  LayerModel(TreeItemModel* psdTreeManager, TreeItemModel* controllerTreeManger,
             QObject* parent = nullptr);

  TreeItemModel* getPsdTreeManager() const;
  TreeItemModel* getControllerTreeManger() const;

  QItemSelectionModel* getPsdTreeSelectionModel() const;
  QItemSelectionModel* getControllerTreeSelectionModel() const;

 public slots:
  /**
   * select the tree layer
   * by call this function it will not emit the selectionChanged signal
   * some layer has the same id in psd tree and controller tree. They will also
   * be selected
   * @param selectionId layerId
   */
  void selectItems(const std::vector<int>& selectionId);
 signals:
  void selectionChanged(const std::vector<int>& selectionId);
 public slots:
  void setItemVisible(const QModelIndex& index, bool visible);
};
}  // namespace ProjectModel
