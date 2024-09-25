#pragma once
#include <QObject>

class QUndoStack;

namespace WaifuL2d {
class MainStageScene;


struct SceneControllerState {
  bool isEdit = false;
};

class SceneController : public QObject {
  Q_OBJECT
  MainStageScene* scene = nullptr;
  SceneControllerState state;
  QUndoStack* editModeUndoStack;

public:
  SceneController(QObject* parent = nullptr);

  QUndoStack* getEditModeUndoStack() const { return editModeUndoStack; }
  void clearUndo() const;

  void setScene(MainStageScene* scene);
  void toggleEditMode();
  bool hasScene() const;

signals:
  void stateChanged(SceneControllerState state);
  void warning(QString message);
};
}

Q_DECLARE_METATYPE(WaifuL2d::SceneControllerState)