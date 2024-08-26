#include "editmodecontroller.h"

#include <QItemSelectionModel>

#include "../model/layer_model.h"
#include "../scene/editmeshcontroller.h"
#include "model/layer.h"
#include "model/scene/mainstagescene.h"
#include "model/scene/meshrendergroup.h"
#include "model/tree_manager.h"
#include "views/mainglstage.h"
#include "views/mainstagesidetoolbar.h"

ProjectModel::BitmapLayer* Controller::EditModeController::getFirstSelectLayer()
    const {
  const auto& layerSelection =
      this->layerModel->getControllerTreeSelectionModel()->selection();
  ProjectModel::BitmapLayer* firstSelectLayer = nullptr;
  // first search selection layer
  for (const auto& index : layerSelection.indexes()) {
    if (!index.isValid()) {
      continue;
    }
    auto item = layerModel->getControllerTreeManger()->itemFromIndex(index);
    if (item->type() == ProjectModel::BitmapLayerType) {
      firstSelectLayer = static_cast<ProjectModel::BitmapLayer*>(item);
      break;
    }
  }

  // if no selection search the first layer of the model
  if (firstSelectLayer == nullptr) {
    this->layerModel->getControllerTreeManger()->forEach([&](const auto& l) {
      if (l->type() == ProjectModel::BitmapLayerType) {
        firstSelectLayer = static_cast<ProjectModel::BitmapLayer*>(l);
        return false;
      }
      return true;
    });
  }
  return firstSelectLayer;
}

Controller::EditModeController::EditModeController(
    Scene::MainStageScene* scene, ProjectModel::LayerModel* layerModel,
    views::MainGlGraphicsView* view, QObject* parent)
    : QObject(parent) {
  this->scene = scene;
  this->layerModel = layerModel;
  this->view = view;
}

void Controller::EditModeController::setDisabledWidget(
    const QList<QWidget*>& widgets) {
  this->disabledWidgets = widgets;
}

void Controller::EditModeController::handleEnterEditMode() {
  auto editLayer = this->getFirstSelectLayer();
  // get the actual mesh to edit
  auto mesh = scene->getRenderGroup()->findMesh(editLayer->getId());
  if (mesh == nullptr) {
    return;
  }

  // create edit controller
  this->currentEditController = new Scene::EditMeshController(
      mesh->getVertices(), mesh->getIncident(), scene->getControllerRoot());
  // add to scene
  scene->getControllerRoot()->addEditMeshController(currentEditController);

  // change selection
  if (!editLayer->data(ProjectModel::VisibleRole).toBool()) {
    layerModel->setItemVisible(editLayer->getId(), true);
  }
  this->layerModel->selectItems({editLayer->getId()});
  this->scene->setSceneMode(Scene::MainStageScene::SceneMode::EDIT);

  this->view->getToolBar()->setEnableTool(2, true);
  for (const auto& disabled_widget : this->disabledWidgets) {
    disabled_widget->setDisabled(true);
  }
}

void Controller::EditModeController::handleLeaveEditMode() {
  this->scene->setSceneMode(Scene::MainStageScene::SceneMode::NORMAL);
  this->view->getToolBar()->setEnableTool(2, false);
  for (const auto& disabled_widget : this->disabledWidgets) {
    disabled_widget->setDisabled(false);
  }

  if (currentEditController != nullptr) {
    this->scene->getControllerRoot()->removeEditMeshController(
        currentEditController);
    delete this->currentEditController;
    this->currentEditController = nullptr;
  }
}