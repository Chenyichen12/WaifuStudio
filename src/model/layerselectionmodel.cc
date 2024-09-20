//
// Created by chenyichen on 2024/9/19.
//
#include "layerselectionmodel.h"

#include "scene/abstractdeformer.h"
#include "tree/layer.h"
#include "tree/layermodel.h"
namespace WaifuL2d {
LayerSelectionModel::LayerSelectionModel(WaifuL2d::LayerModel *model,
                                         MainStageScene *scene)
    : QItemSelectionModel(model),model(model), scene(scene) {}

void LayerSelectionModel::select(const QItemSelection &selection,
                                 QItemSelectionModel::SelectionFlags command) {
  QList<Layer *> layers;
  for (auto &index : selection.indexes()) {
    auto layer = model->layerFromIndex(index);
    if (layer) {
      layers.append(layer);
    }
  }

  QItemSelectionModel::select(selection, command);
}
}  // namespace WaifuL2d
