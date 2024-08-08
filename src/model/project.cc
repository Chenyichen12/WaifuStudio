//
// Created by chenyichen on 7/31/24.
//
#include "project.h"
#include "layer.h"
#include "layer_model.h"
#include "mainstagescene.h"
#include "mesh.h"
#include "spriterendergroup.h"
#include "tree_manager.h"
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
void ProjectBuilder::setUpScene() {
  auto tree = this->model->getControllerTreeManger();
  auto mainRenderGroup = new SpriteRenderGroup(projectWidth, projectHeight);
  tree->forEach([&](QStandardItem* item) {
    if (item->type() == LayerTypes::BitmapLayerType) {
      auto bitmapLayer = static_cast<BitmapLayer*>(item);
      auto builder = MeshBuilder();
      builder.setUpProjectRect(QRect(0, 0, projectWidth, projectHeight));
      auto bitmap = manager->getBitmap(bitmapLayer->getBitmapId());
      builder.setUpDefault(bitmap);
      auto mesh = builder.extractMesh();

      // store id of the layer
      mesh->bindId(bitmapLayer->getId());

      mainRenderGroup->pushFrontMesh(mesh);
    }
    return true;
  });
  this->sceneModel =
      new MainStageScene(projectWidth, projectHeight, mainRenderGroup);
}

}  // namespace ProjectModel