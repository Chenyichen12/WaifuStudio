#include "scenecontroller.h"

#include <qcoreapplication.h>

#include <QMessageBox>

#include "../scene/mainstagescene.h"
#include "../scene/abstractdeformer.h"
#include "model/scene/deformer/meshdeformer.h"

namespace WaifuL2d {
SceneController::SceneController(QObject* parent): QObject(parent) {
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
  } else {
    qDebug() << "edit state off";
  }

  this->state.isEdit = !this->state.isEdit;
  emit stateChanged(this->state);
}


bool SceneController::hasScene() const { return scene != nullptr; }
}