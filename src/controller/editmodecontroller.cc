#include "editmodecontroller.h"

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QUndoCommand>

#include "model/layer.h"
#include "model/layer_model.h"
#include "model/scene/editmeshcontroller.h"
#include "model/scene/mainstagescene.h"
#include "model/scene/meshrendergroup.h"
#include "model/tree_manager.h"
#include "views/mainglstage.h"
#include "views/mainstagesidetoolbar.h"
#include "views/mainstagetopbar.h"
namespace Controller {
/**
 * the command when controller enter edit mode
 */
class AbstractSceneEditCommand : public QUndoCommand {
 private:
  Scene::MainStageScene* scene;
  bool ifOwnController = true;

 protected:
  Scene::EditMeshController* newController;
  virtual void leave() {
    scene->setSceneMode(Scene::MainStageScene::SceneMode::NORMAL);
    newController->setParentItem(nullptr);
    newController->setControllerParent(nullptr);
    scene->removeItem(newController);
    ifOwnController = true;
  }
  virtual void enter() {
    scene->getControllerRoot()->unSelectAllController();
    scene->setSceneMode(Scene::MainStageScene::SceneMode::EDIT);
    newController->setParentItem(scene->getControllerRoot());
    newController->setControllerParent(scene->getControllerRoot());

    // if add to scene it will no longer own the controller
    ifOwnController = false;
  }

 public:
  AbstractSceneEditCommand(Scene::MainStageScene* scene,
                           Scene::EditMeshController* controller,
                           QUndoCommand* parent = nullptr)
      : QUndoCommand(parent), scene(scene), newController(controller) {}
  ~AbstractSceneEditCommand() override {
    if (ifOwnController) {
      delete newController;
    }
  }

  void redo() override = 0;
  void undo() override = 0;
};
class SceneEditCommand : public AbstractSceneEditCommand {
 private:
  EditModeController* controller;

 protected:
  bool isLeave = true;
  void leave() override {
    AbstractSceneEditCommand::leave();
    controller->setCurrentEditMeshController(nullptr);
  }
  void enter() override {
    AbstractSceneEditCommand::enter();
    controller->setCurrentEditMeshController(newController);
  }

 public:
  SceneEditCommand(Scene::MainStageScene* scene,
                   Scene::EditMeshController* controller,
                   EditModeController* editModeController,
                   QUndoCommand* parent = nullptr)
      : AbstractSceneEditCommand(scene, controller, parent),
        controller(editModeController) {}
  void setIsLeave(bool isLeave) { this->isLeave = isLeave; }
  void redo() override { isLeave ? leave() : enter(); }
  void undo() override { isLeave ? enter() : leave(); }
};

class UiEditCommand : QUndoCommand {
 protected:
  void leave() {
    if (view) {
      view->getToolBar()->setEnableTool(2, false);
    }
    if (topBar) {
      topBar->setEditBtnChecked(false);
    }
    for (const auto& disabled_widget : disabledWidgets) {
      disabled_widget->setDisabled(false);
    }
  }
  void enter() {
    if (view) {
      view->getToolBar()->setEnableTool(2, true);
    }
    if (topBar) {
      topBar->setEditBtnChecked(true);
    }
    for (const auto& disabled_widget : disabledWidgets) {
      disabled_widget->setDisabled(true);
    }
  }

 public:
  UiEditCommand(QUndoCommand* parent = nullptr) : QUndoCommand(parent) {}
  bool ifEnter = true;
  views::MainGlGraphicsView* view = nullptr;
  views::MainStageTopBar* topBar = nullptr;
  QList<QWidget*> disabledWidgets;
  void redo() override {
    if (ifEnter) {
      enter();
    } else {
      leave();
    }
  }
  void undo() override {
    if (ifEnter) {
      leave();
    } else {
      enter();
    }
  }
};

ProjectModel::BitmapLayer* EditModeController::getFirstSelectLayer() const {
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

bool warnUndoClear() {
  QMessageBox msgBox;
  msgBox.setText("The undo stack will be cleared, do you want to continue?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  return msgBox.exec() == QMessageBox::Yes;
}

void EditModeController::handleFailLeaveEditMode() const {
  QMessageBox::warning(nullptr, "Warning", "Mesh not complete");
  if (topBar) {
    topBar->setEditBtnChecked(true);
  }
}

UiEditCommand* EditModeController::createUiEditCommand(
    QUndoCommand* parent) const {
  auto uiEditCommand = new UiEditCommand(parent);
  uiEditCommand->topBar = this->topBar;
  uiEditCommand->view = this->view;
  uiEditCommand->disabledWidgets = this->disabledWidgets;
  return uiEditCommand;
}

EditModeController::EditModeController(Scene::MainStageScene* scene,
                                       ProjectModel::LayerModel* layerModel,
                                       QObject* parent)
    : QObject(parent) {
  this->scene = scene;
  this->layerModel = layerModel;
}

EditModeController::~EditModeController() = default;

void EditModeController::setCurrentEditMeshController(
    Scene::EditMeshController* currentEditMesh) {
  currentEditMeshController = currentEditMesh;
}

void EditModeController::setDisabledWidget(const QList<QWidget*>& widgets) {
  this->disabledWidgets = widgets;
}

void EditModeController::setView(views::MainGlGraphicsView* view) {
  this->view = view;
}

void EditModeController::setTopBar(views::MainStageTopBar* topBar) {
  this->topBar = topBar;
  connect(topBar, &views::MainStageTopBar::enterEditMode, this,
          &Controller::EditModeController::handleEnterEditMode);
  connect(topBar, &views::MainStageTopBar::leaveEditMode, this,
          &Controller::EditModeController::handleLeaveEditMode);
}

void EditModeController::setUndoStack(QUndoStack* stack) {
  this->undoStack = stack;
}

void EditModeController::handleEnterEditMode() {
  auto editLayer = this->getFirstSelectLayer();
  // get the actual mesh to edit
  auto mesh = scene->getRenderGroup()->findMesh(editLayer->getId());
  if (mesh == nullptr) {
    if (topBar) {
      topBar->setEditBtnChecked(false);
    }
    return;
  }
  // change selection
  this->layerModel->selectItems({});

  auto commandGroup = std::make_unique<QUndoCommand>();
  // create edit controller
  auto editController = new Scene::EditMeshController(
      mesh->getVertices(), mesh->getIncident(), scene->getControllerRoot());
  // add to scene
  currentEditMeshController = editController;
  auto sceneCommand =  new SceneEditCommand(scene, editController, this, commandGroup.get());
  sceneCommand->setIsLeave(false);
  createUiEditCommand(commandGroup.get());

  if (undoStack) {
    undoStack->push(commandGroup.release());
  } else {
    commandGroup->redo();
  }
}

void EditModeController::handleLeaveEditMode() {
  if (currentEditMeshController != nullptr &&
      !currentEditMeshController->ifValidTriangle()) {
    this->handleFailLeaveEditMode();
    return;
  }

  // if (!warnUndoClear()) {
  //   if (topBar) {
  //     topBar->setEditBtnChecked(true);
  //   }
  //   return;
  // }

  auto commandGroup = std::make_unique<QUndoCommand>();
  auto sceneCommand = new SceneEditCommand(scene, currentEditMeshController,
                                           this, commandGroup.get());

  auto uiCommand = createUiEditCommand(commandGroup.get());
  uiCommand->ifEnter = false;
  if (undoStack) {
    undoStack->push(commandGroup.release());
  } else {
    commandGroup->redo();
  }
}
}  // namespace Controller