//
// Created by chenyichen on 7/31/24.
//
#include "model/project.h"
#include "model/layer_model.h"
#include "model/layer_bitmap.h"


namespace ProjectModel {
Project* ProjectBuilder::build() const {
  if (this->manager == nullptr || this->model == nullptr) {
    return nullptr;
  }

  auto project = new Project();
  project->layerModel = this->model;
  project->bitmapManager = this->manager;

  model->setParent(project);
  manager->setParent(project);

  return project;
}
void ProjectBuilder::setBitmapManager(BitmapManager* m) { this->manager = m; }
void ProjectBuilder::setLayerModel(LayerModel* m) { this->model = m; }

}  // namespace ProjectModel

