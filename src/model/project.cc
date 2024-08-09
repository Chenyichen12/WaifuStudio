//
// Created by chenyichen on 7/31/24.
//
#include "project.h"
#include "layer.h"
#include "layer_model.h"
#include "scene/mainstagescene.h"
#include "scene/mesh.h"
#include "scene/meshrendergroup.h"
#include "scene/scenecontrollergroup.h"
#include "scene/scenecontroller.h"
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

Scene::MainStageScene* Project::getScene() const { return scene; }

void ProjectBuilder::setUpScene() {
  auto tree = this->model->getControllerTreeManger();
  auto mainRenderGroup = new Scene::MeshRenderGroup(projectWidth, projectHeight);
  auto controllerGroup = new Scene::SceneControllerGroup();
  tree->forEach([&](QStandardItem* item) {
    if (item->type() == LayerTypes::BitmapLayerType) {
      auto bitmapLayer = static_cast<BitmapLayer*>(item);
      auto builder = Scene::MeshBuilder();
      auto bitmap = manager->getBitmap(bitmapLayer->getBitmapId());
      builder.setUpDefault(bitmap);
      auto mesh = builder.buildMesh().release();

      // store id of the layer
      mesh->bindId(bitmapLayer->getId());
      auto meshController = new Scene::MeshController(mesh);

      mainRenderGroup->pushFrontMesh(mesh);
      controllerGroup->pushFrontController(meshController);
    }
    return true;
  });
  this->sceneModel =
      new Scene::MainStageScene(mainRenderGroup,controllerGroup);
}

}  // namespace ProjectModel