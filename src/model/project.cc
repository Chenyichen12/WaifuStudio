//
// Created by chenyichen on 7/31/24.
//
#include "project.h"

#include <stack>

#include <QUndoStack>
#include "layer.h"
#include "layer_model.h"
#include "scene/mainstagescene.h"
#include "scene/mesh.h"
#include "scene/meshrendergroup.h"
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
  project->undoStack = new QUndoStack();
  project->undoStack->setParent(project);

  model->setParent(project);
  model->setUndoStack(project->undoStack);

  manager->setParent(project);
  sceneModel->setParent(project);

  // connect model communication
  // two-way select connect signal
  QObject::connect(model, &LayerModel::selectionChanged, sceneModel,
                   &Scene::MainStageScene::selectLayers);
  QObject::connect(sceneModel, &Scene::MainStageScene::selectionChanged, model,
                     &LayerModel::selectItems);

  // set visible connect signal
  QObject::connect(model, &LayerModel::visibleChanged, sceneModel,
                     &Scene::MainStageScene::setVisibleOfLayer);
  return project;
}
void ProjectBuilder::setBitmapManager(BitmapManager* m) { this->manager = m; }
void ProjectBuilder::setLayerModel(LayerModel* m) { this->model = m; }

Scene::MainStageScene* Project::getScene() const { return scene; }

void ProjectBuilder::setUpScene() {
  auto tree = this->model->getControllerTreeManger();
  // gl render layer setup
  auto mainRenderGroup =
      new Scene::MeshRenderGroup(projectWidth, projectHeight);
  // controller setup
  auto controllerRoot = new Scene::RootController(projectWidth, projectHeight);
  auto controllerStack =
      std::stack<std::pair<QStandardItem*, Scene::AbstractController*>>();
  controllerStack.emplace(tree->invisibleRootItem(), controllerRoot);

  tree->forEach([&](QStandardItem* item) {
    if (item->type() == LayerTypes::BitmapLayerType) {
      auto bitmapLayer = static_cast<BitmapLayer*>(item);
      auto builder = Scene::MeshBuilder();
      auto bitmap = manager->getBitmap(bitmapLayer->getBitmapId());
      builder.setUpDefault(bitmap);
      auto mesh = builder.buildMesh().release();

      // store id of the layer
      mesh->bindId(bitmapLayer->getId());
      if (item->parent() != controllerStack.top().first && item->parent() != nullptr) {
        controllerStack.pop();
      }

      // has set parent will not leak memory
      auto meshController = new Scene::MeshController(mesh,controllerRoot);
      meshController->setControllerParent(controllerStack.top().second);
      mainRenderGroup->pushFrontMesh(mesh);
    }
    return true;
  });
  this->sceneModel =
      new Scene::MainStageScene(mainRenderGroup, controllerRoot);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Project::undo() { undoStack->undo(); }
// ReSharper disable once CppMemberFunctionMayBeConst
void Project::redo() { undoStack->redo(); }
}  // namespace ProjectModel