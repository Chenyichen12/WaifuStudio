//
// Created by chenyichen on 7/31/24.
//
#include "project.h"

#include "layer_bitmap.h"
#include "layer_model.h"
#include "mainstagescene.h"

namespace ProjectModel {
Project* ProjectBuilder::build() {
  if (this->manager == nullptr || this->model == nullptr) {
    return nullptr;
  }
  this->setUpScene();

  auto project = new Project();
  project->layerModel = this->model;
  project->bitmapManager = this->manager;
  project->scene = sceneModel;

  model->setParent(project);
  manager->setParent(project);
  sceneModel->setParent(project);

  return project;
}
void ProjectBuilder::setBitmapManager(BitmapManager* m) { this->manager = m; }
void ProjectBuilder::setLayerModel(LayerModel* m) { this->model = m; }
void ProjectBuilder::setUpScene() { this->sceneModel = new MainStageScene(); }

}  // namespace ProjectModel
