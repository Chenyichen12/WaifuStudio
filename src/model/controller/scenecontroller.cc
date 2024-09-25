#include "scenecontroller.h"
#include "../scene/mainstagescene.h"

namespace WaifuL2d {
SceneController::SceneController(QObject* parent): QObject(parent) {
}

void SceneController::setScene(MainStageScene* scene) {
  this->scene = scene;
  emit stateChanged({});
}

void SceneController::toggleEditMode() {
  // if (!this->scene)return;
  this->state.isEdit = !this->state.isEdit;
  emit stateChanged(this->state);
}


bool SceneController::hasScene() const { return scene != nullptr; }
}