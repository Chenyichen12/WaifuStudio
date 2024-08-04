//
// Created by chenyichen on 7/31/24.
//
#pragma once
#include "QObject"
namespace ProjectModel {
class LayerModel;
class BitmapManager;
class Project;
class MainStageScene;

class ProjectBuilder {
 private:
  BitmapManager* manager = nullptr;
  LayerModel* model = nullptr;
  MainStageScene* sceneModel = nullptr;

 protected:
  void setUpScene();

 public:
  Project* build();
  void setBitmapManager(BitmapManager*);
  void setLayerModel(LayerModel*);
  ProjectBuilder() = default;
};
class Project : public QObject {
  friend ProjectBuilder;

 private:
  Project() = default;
  BitmapManager* bitmapManager = nullptr;
  LayerModel* layerModel = nullptr;
  MainStageScene* scene = nullptr;

 public:
  BitmapManager* getBitmapManager() const { return bitmapManager; }
  LayerModel* getLayerModel() const { return layerModel; }
  MainStageScene* getScene() const { return scene; }
};

}  // namespace ProjectModel
