//
// Created by chenyichen on 2024/9/19.
//
#include "layerselectionmodel.h"

#include "scene/abstractdeformer.h"
#include "scene/deformmanager.h"
#include "tree/layer.h"
#include "tree/layermodel.h"
namespace WaifuL2d {
LayerSelectionModel::LayerSelectionModel(WaifuL2d::LayerModel *model,
                                         WaifuL2d::DeformManager *deformManager)
    : QItemSelectionModel(model) {
  this->model = model;
  this->deformManager = deformManager;

  connect(deformManager, &DeformManager::deformShouldSelect, this,
          [this](QList<AbstractDeformer *> deformers) {
            QItemSelection selection;
            for (auto &deformer : deformers) {
              auto layer = deformer->getBindLayer();
              auto index = this->model->indexFromItem(layer);
              selection.select(index, index);
            }
            this->select(selection,QItemSelectionModel::ClearAndSelect);
          });
}

void LayerSelectionModel::select(const QItemSelection &selection,
                                 QItemSelectionModel::SelectionFlags command) {
  QList<Layer *> layers;
  for (auto &index : selection.indexes()) {
    auto layer = model->layerFromIndex(index);
    if (layer) {
      layers.append(layer);
    }
  }

  deformManager->selectFromLayers(layers);
  QItemSelectionModel::select(selection, command);
}
}  // namespace WaifuL2d
