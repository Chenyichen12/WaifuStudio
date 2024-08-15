#pragma once
#include <QObject>
#include <QModelIndexList>
class QUndoStack;
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

  QUndoStack* undoStack = nullptr;
 public:
  LayerModel(TreeItemModel* psdTreeManager, TreeItemModel* controllerTreeManger,
             QObject* parent = nullptr);

  void setUndoStack(QUndoStack* stack);

  TreeItemModel* getPsdTreeManager() const;
  TreeItemModel* getControllerTreeManger() const;

  QItemSelectionModel* getPsdTreeSelectionModel() const;
  QItemSelectionModel* getControllerTreeSelectionModel() const;
  /**
   * set the visible of the layer
   * @param id 
   * @param visible 
   */
  void setItemVisible(int id, bool visible);

 public slots:
  /**
   * select the tree layer
   * by call this function it will not emit the selectionChanged signal
   * some layer has the same id in psd tree and controller tree. They will also
   * be selected
   * @param selectionId layerId
   */
  void selectItems(const std::vector<int>& selectionId);

  /**
   * handle toggle the visible from view
   * @param index 
   * @param visible 
   */
  void handleItemSetVisible(const QModelIndex& index, bool visible);
  void handleVisibleSelectEnd(const QModelIndexList& changeLists);
 signals:
  void selectionChanged(const std::vector<int>& selectionId);
  void visibleChanged(int id, bool visible);
  
};
}  // namespace ProjectModel
