#include "scenecontroller.h"
#include "model/scene/mainstagescene.h"
#include "model/scene/abstractdeformer.h"
#include "model/scene/deformer/meshdeformer.h"
#include "mesheditor.h"
#include <QUndoStack>

namespace WaifuL2d {
SceneController::SceneController(QObject* parent): QObject(parent) {
  this->editModeUndoStack = new QUndoStack(this);
}

void SceneController::clearUndo() const {
  editModeUndoStack->clear();
}

void SceneController::setScene(MainStageScene* curScene) {
  this->scene = curScene;
  this->state = {};
  emit stateChanged(state);
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
    // push owner to scene
    scene->addItem(state.editor);
    state.editor->setZValue(3);
    state.editor->setHandleRect(scene->getProjectRect());
    // the editor may push changes to the undo stack
    // may be moved to another function, but it is clear for now
    connect(state.editor, &MeshEditor::editorCommand, this,
            [this](const std::shared_ptr<MeshEditorCommand>& command) {
              auto undoCommand = command->createUndoCommand();
              this->editModeUndoStack->push(undoCommand);
            });
  } else {
    Q_ASSERT(state.editor != nullptr);
    // delete it when edit off
    delete state.editor;
    clearUndo(); // clean the undo stack when every edit off
    state.editor = nullptr;
  }

  this->state.isEdit = !this->state.isEdit;
  emit stateChanged(this->state);
}


bool SceneController::hasScene() const { return scene != nullptr; }

void SceneController::setEditTool(EditToolType type) {
  Q_ASSERT(state.isEdit);
  if (state.editTool == type) {
    // return when type is not changed
    return;
  }
  state.editTool = type;
  state.editor->setEditTool(type);
  emit stateChanged(state);
}
}