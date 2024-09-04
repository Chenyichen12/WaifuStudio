//
// Created by chenyichen on 7/31/24.
//
#pragma once
#include "QObject"
class QUndoStack;

namespace Scene {
class MainStageScene;
}
namespace ProjectModel {
class LayerModel;
class BitmapManager;
class Project;

/**
 * the builder to build project
 */
class ProjectBuilder {
 private:
  BitmapManager* manager = nullptr;
  LayerModel* model = nullptr;
  Scene::MainStageScene* sceneModel = nullptr;

 protected:
  /**
   * the default function to set up a scene
   * use the default way to create mesh
   */
  void setUpScene();

 public:
  int projectWidth;
  int projectHeight;

  Project* build();
  void setBitmapManager(BitmapManager*);
  void setLayerModel(LayerModel*);
  ProjectBuilder() = default;
};

/**
 * the project model
 * contains all model data in project
 */
class Project : public QObject {
  friend ProjectBuilder;

 private:
  Project() = default;
  BitmapManager* bitmapManager = nullptr;
  LayerModel* layerModel = nullptr;
  Scene::MainStageScene* scene = nullptr;
  QUndoStack* undoStack;

 public:
  BitmapManager* getBitmapManager() const { return bitmapManager; }
  LayerModel* getLayerModel() const { return layerModel; }
  Scene::MainStageScene* getScene() const;
  QUndoStack* getUndoStack() const { return undoStack; }
  void undo();
  void redo();
};

}  // namespace ProjectModel
