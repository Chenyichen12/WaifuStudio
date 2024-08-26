#include "editmodecontroller.h"

#include <QItemSelectionModel>

#include "../model/layer_model.h"
#include "model/layer.h"
#include "model/scene/mainstagescene.h"
#include "model/tree_manager.h"
#include "views/mainglstage.h"
#include "views/mainstagesidetoolbar.h"

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

void Controller::EditModeController::handleEnterEditMode() const {
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

  // TODO: complete get first select layer
  qDebug() << firstSelectLayer->text();

  this->scene->setSceneMode(Scene::MainStageScene::SceneMode::EDIT);
  this->view->getToolBar()->setEnableTool(2, true);
  for (const auto& disabled_widget : this->disabledWidgets) {
    disabled_widget->setDisabled(true);
  }
}

void Controller::EditModeController::handleLeaveEditMode() const {
  this->scene->setSceneMode(Scene::MainStageScene::SceneMode::NORMAL);
  this->view->getToolBar()->setEnableTool(2, false);
  for (const auto& disabled_widget : this->disabledWidgets) {
    disabled_widget->setDisabled(false);
  }
}