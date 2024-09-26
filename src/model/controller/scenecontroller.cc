#include "scenecontroller.h"
#include "../scene/mainstagescene.h"
#include "../scene/abstractdeformer.h"
#include "model/scene/deformer/meshdeformer.h"
#include "../scene/deformer/mesheditor.h"
#include <QUndoStack>

namespace WaifuL2d {
SceneController::SceneController(QObject* parent): QObject(parent) {
  this->editModeUndoStack = new QUndoStack(this);
}

void SceneController::clearUndo() const {
  editModeUndoStack->clear();
}

void SceneController::setScene(MainStageScene* scene) {
  this->scene = scene;
  emit stateChanged({});
}

void SceneController::toggleEditMode() {
  Q_ASSERT(scene != nullptr);

  if (!this->state.isEdit) {
    MeshDeformer* editDeformer = nullptr;
    for (auto item : scene->getSelectedDeformers()) {
      if (item->type() == AbstractDeformer::MeshDeformerType) {
        editDeformer = static_cast<MeshDeformer*>(item);
        break;
      }
    }

    if (editDeformer == nullptr) {
      emit warning("Please select a mesh deformer first.");
      return;
    }

    state.editor =
        new MeshEditor(editDeformer->getScenePoints(),
                       editDeformer->getMeshIncident());
    scene->addItem(state.editor);
    state.editor->setZValue(3);

    // may need to emit some signal to tell edit mode
  } else {
    Q_ASSERT(state.editor != nullptr);
    delete state.editor;
    state.editor = nullptr;
  }

  this->state.isEdit = !this->state.isEdit;
  emit stateChanged(this->state);
}


bool SceneController::hasScene() const { return scene != nullptr; }
}