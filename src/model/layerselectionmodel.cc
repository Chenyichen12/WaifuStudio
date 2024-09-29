//
// Created by chenyichen on 2024/9/19.
//
#include "layerselectionmodel.h"

#include "scene/abstractdeformer.h"
#include "scene/mainstagescene.h"
#include "tree/layer.h"
#include "tree/layermodel.h"
namespace WaifuL2d {
LayerSelectionModel::LayerSelectionModel(WaifuL2d::LayerModel *model,
                                         MainStageScene *scene)
    : QItemSelectionModel(model), model(model), scene(scene) {
  connect(scene, &MainStageScene::shouldSelectDeformers, this,
          &LayerSelectionModel::selectById);
}

void LayerSelectionModel::selectById(const QList<int>& ids) {
  QItemSelection selection;
  for (auto id : ids) {
    auto layer = this->model->layerFromId(id);
    if (layer) {
      selection.select(layer->index(), layer->index());
    }
  }
  this->select(selection, QItemSelectionModel::ClearAndSelect);
}

void LayerSelectionModel::setEnable(bool enabled) { this->enabled = enabled; }

void LayerSelectionModel::select(const QItemSelection &selection,
                                 QItemSelectionModel::SelectionFlags command) {
  if (!this->enabled) {
    return;
  }
  QList<int> ids;
  for (auto &index : selection.indexes()) {
    auto layer = model->layerFromIndex(index);
    if (layer) {
      ids.append(layer->getId());
    }
  }
  scene->selectDeformersById(ids);

  QItemSelectionModel::select(selection, command);
}
}  // namespace WaifuL2d
