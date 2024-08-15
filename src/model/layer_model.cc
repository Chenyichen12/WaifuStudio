#include "layer_model.h"

#include <QItemSelectionModel>
#include <QUndoStack>
#include "layer.h"
#include "tree_manager.h"
namespace ProjectModel {

struct VisibleUndoInfo {
  int id;
  bool visible;
};
class LayerVisibleCommand : public QUndoCommand {
  QList<VisibleUndoInfo> info;
  LayerModel *model;
  bool first = true;

 public:
  LayerVisibleCommand(LayerModel *model, const QList<VisibleUndoInfo> &info,
                      QUndoCommand *parent = nullptr)
      : QUndoCommand(parent), info(info) {
    this->model = model;
  }

  void redo() override {
    if (first) {
      first = false;
      return;
    }

    for (const auto &visible_undo_info : info) {
      model->setItemVisible(visible_undo_info.id, visible_undo_info.visible);
    }
  }

  void undo() override {
    for (const auto &visible_undo_info : info) {
      model->setItemVisible(visible_undo_info.id, !visible_undo_info.visible);
    }
  }
};

void LayerModel::handleSelectionChanged(const QItemSelection &selected,
                                        const QItemSelection &deselected) {
  auto which = static_cast<QItemSelectionModel *>(sender());
  auto idList = std::vector<int>();
  for (const auto &selected_index : which->selectedIndexes()) {
    idList.emplace_back(selected_index.data(UserIdRole).toInt());
  }
  emit selectionChanged(idList);
}

LayerModel::LayerModel(TreeItemModel *psdTreeManager,
                       TreeItemModel *controllerTreeManger, QObject *parent)
    : QObject(parent) {
  this->controllerTreeManger = controllerTreeManger;
  this->psdTreeManager = psdTreeManager;
  psdTreeManager->setParent(this);
  controllerTreeManger->setParent(this);

  this->psdTreeSelectionModel = new QItemSelectionModel(psdTreeManager);
  this->controllerTreeSelectionModel =
      new QItemSelectionModel(controllerTreeManger);
  connect(psdTreeSelectionModel, &QItemSelectionModel::selectionChanged, this,
          &LayerModel::handleSelectionChanged, Qt::DirectConnection);
  connect(controllerTreeSelectionModel, &QItemSelectionModel::selectionChanged,
          this, &LayerModel::handleSelectionChanged, Qt::DirectConnection);
}

void LayerModel::setUndoStack(QUndoStack *stack) { this->undoStack = stack; }

TreeItemModel *LayerModel::getPsdTreeManager() const { return psdTreeManager; }
TreeItemModel *LayerModel::getControllerTreeManger() const {
  return controllerTreeManger;
}

QItemSelectionModel *LayerModel::getPsdTreeSelectionModel() const {
  return psdTreeSelectionModel;
}

QItemSelectionModel *LayerModel::getControllerTreeSelectionModel() const {
  return controllerTreeSelectionModel;
}

void LayerModel::setItemVisible(int id, bool visible) {
  auto n = controllerTreeManger->findNode(id);
  if (n != nullptr) {
    n->setData(visible, VisibleRole);
  }
  auto c = psdTreeManager->findNode(id);
  if (c != nullptr) {
    c->setData(visible, VisibleRole);
  }

  emit visibleChanged(id, visible);
}

void LayerModel::selectItems(const std::vector<int> &selectionId) {
  // we need to do this to defence loop
  blockSignals(true);
  auto treeSelection = QItemSelection();
  auto controllerSelection = QItemSelection();
  for (int selection_id : selectionId) {
    auto treeNode = psdTreeManager->findNode(selection_id);
    if (treeNode != nullptr) {
      treeSelection.select(treeNode->index(), treeNode->index());
    }
    auto controllerNode = controllerTreeManger->findNode(selection_id);
    if (controllerNode != nullptr) {
      controllerSelection.select(controllerNode->index(),
                                 controllerNode->index());
    }
  }
  psdTreeSelectionModel->select(treeSelection,
                                QItemSelectionModel::ClearAndSelect);
  controllerTreeSelectionModel->select(controllerSelection,
                                       QItemSelectionModel::ClearAndSelect);

  blockSignals(false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LayerModel::handleItemSetVisible(const QModelIndex &index, bool visible) {
  this->setItemVisible(index.data(UserIdRole).toInt(), visible);
}

void LayerModel::handleVisibleSelectEnd(const QModelIndexList &changeLists) {

  // should push undo to stack
  QList<VisibleUndoInfo> infoList;
  for (const auto &changeItem : changeLists) {
    auto id = changeItem.data(UserIdRole).toInt();
    auto visible = changeItem.data(VisibleRole).toBool();
    auto info = VisibleUndoInfo{id, visible};
    infoList.push_back(info);
  }
  if(undoStack != nullptr) {
    undoStack->push(new LayerVisibleCommand(this, infoList));
  }
}
}  // namespace ProjectModel
