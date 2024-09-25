#pragma once
#include <QObject>

namespace WaifuL2d {
class MainStageScene;


struct SceneControllerState {
  bool isEdit = false;
};

class SceneController : public QObject {
  Q_OBJECT
  MainStageScene* scene = nullptr;
  SceneControllerState state;

public:
  SceneController(QObject* parent = nullptr);
  void setScene(MainStageScene* scene);
  void toggleEditMode();
  bool hasScene() const;

signals:
  void stateChanged(SceneControllerState state);
};
}

Q_DECLARE_METATYPE(WaifuL2d::SceneControllerState)