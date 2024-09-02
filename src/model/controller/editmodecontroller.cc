#include "editmodecontroller.h"

#include <QItemSelectionModel>
#include <QMessageBox>

#include "../model/layer_model.h"
#include "../scene/editmeshcontroller.h"
#include "model/layer.h"
#include "model/scene/mainstagescene.h"
#include "model/scene/meshrendergroup.h"
#include "model/tree_manager.h"
#include "views/mainglstage.h"
#include "views/mainstagesidetoolbar.h"
#include "views/mainstagetopbar.h"

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

void Controller::EditModeController::handleFailLeaveEditMode() const {
  QMessageBox::warning(nullptr, "Warning", "Mesh not complete");
  if (topBar) {
    topBar->setEditBtnChecked(true); 
  }
}

Controller::EditModeController::EditModeController(
    Scene::MainStageScene* scene, ProjectModel::LayerModel* layerModel,
    QObject* parent)
    : QObject(parent) {
  this->scene = scene;
  this->layerModel = layerModel;
}

Controller::EditModeController::~EditModeController() = default;

void Controller::EditModeController::setDisabledWidget(
    const QList<QWidget*>& widgets) {
  this->disabledWidgets = widgets;
}

void Controller::EditModeController::setView(views::MainGlGraphicsView* view) {
  this->view = view;
}

void Controller::EditModeController::setTopBar(views::MainStageTopBar* topBar) {
  this->topBar = topBar;
  connect(topBar, &views::MainStageTopBar::enterEditMode,
          this, &Controller::EditModeController::handleEnterEditMode);
  connect(topBar, &views::MainStageTopBar::leaveEditMode, this,
          &Controller::EditModeController::handleLeaveEditMode);
}


void Controller::EditModeController::handleEnterEditMode() {
  auto editLayer = this->getFirstSelectLayer();
  // get the actual mesh to edit
  auto mesh = scene->getRenderGroup()->findMesh(editLayer->getId());
  if (mesh == nullptr) {
    topBar->setEditBtnChecked(false);
    if (topBar) {
      topBar->setEditBtnChecked(false);
    }
    return;
  }

  // create edit controller
  // root will manage the controller so not need to delete
  auto editController = new Scene::EditMeshController(
      mesh->getVertices(), mesh->getIncident(), scene->getControllerRoot());
  // add to scene
  scene->getControllerRoot()->setEditMeshController(editController);
  currentEditMeshController = editController;

  // change selection
  if (!editLayer->data(ProjectModel::VisibleRole).toBool()) {
    layerModel->setItemVisible(editLayer->getId(), true);
  }
  this->layerModel->selectItems({editLayer->getId()});
  this->scene->setSceneMode(Scene::MainStageScene::SceneMode::EDIT);

  if (view != nullptr) view->getToolBar()->setEnableTool(2, true);

  for (const auto& disabled_widget : this->disabledWidgets) {
    disabled_widget->setDisabled(true);
  }
}

void Controller::EditModeController::handleLeaveEditMode() {
  if (currentEditMeshController != nullptr &&
      !currentEditMeshController->ifValidTriangle()) {
    this->handleFailLeaveEditMode();
    return;
  }
  this->scene->setSceneMode(Scene::MainStageScene::SceneMode::NORMAL);
  if (view != nullptr) this->view->getToolBar()->setEnableTool(2, false);

  for (const auto& disabled_widget : this->disabledWidgets) {
    disabled_widget->setDisabled(false);
  }
  this->scene->getControllerRoot()->removeEditMeshController();
}