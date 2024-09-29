//
// Created by chenyichen on 2024/9/19.
//

#pragma once
#include <QItemSelectionModel>

namespace WaifuL2d {
class LayerModel;
class MainStageScene;

class LayerSelectionModel : public QItemSelectionModel {
private:
  LayerModel* model;
  MainStageScene* scene;

  bool enabled;

public:
  LayerSelectionModel(LayerModel* model, MainStageScene* scene);

  void selectById(const QList<int>& id);


  void setEnable(bool enabled);

public slots:
  void select(const QItemSelection& selection,
              QItemSelectionModel::SelectionFlags command) override;
};
} // namespace WaifuL2d