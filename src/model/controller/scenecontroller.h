#pragma once
#include <QObject>

class QUndoStack;

namespace WaifuL2d {
class MainStageScene;
class MeshDeformer;
class MeshEditor;
class EditFinishCommandWrapper;

enum EditToolType {
  Cursor = 0,
  AddPen = 1,
  RemovePen = 2,
};

class SceneController;

struct SceneControllerState {
  friend SceneController;

private:
  MeshEditor* editor = nullptr;
  MeshDeformer* editDeformer = nullptr;

public:
  bool isEdit = false;
  EditToolType editTool = Cursor;
};

/**
 * To control scene event like enter edit event change tool event and so on.
 * the ui should call scene api through this controller
 */
class SceneController : public QObject {
  Q_OBJECT

  // the current scene
  MainStageScene* scene = nullptr;
  SceneControllerState state;
  QUndoStack* editModeUndoStack;

public:
  SceneController(QObject* parent = nullptr);

  /**
   * the edit undo stack is different from main undoStack
   * in the edit mode. it will record the edit deformer action
   * @return the edit mode undoStack
   */
  QUndoStack* getEditModeUndoStack() const { return editModeUndoStack; }
  void clearUndo() const;

  /**
   * won't take the ownership of scene
   * make sure call it before call other function
   * @param scene the current scene
   */
  void setScene(MainStageScene* scene);

  /**
   * handle the event when ui toggle the edit btn
   */
  void toggleEditMode();

  /**
   * @return has current scene
   */
  bool hasScene() const;

  /**
   * change the edit tool of the scene
   * make sure to call it when state.isEdit is true
   * @param type tool type
   */
  void setEditTool(EditToolType type);

  /**
   * init scene gl
   * call it before set scene for graphics view
   */
  void initGL() const;
signals:
  void stateChanged(const SceneControllerState& state);
  void warning(const QString& message);
  void editFinishCommand(std::shared_ptr<EditFinishCommandWrapper> command);

  /**
   * same as state change. isEdit == state.isEdit
   * @param isEdit 
   */
  void editModeChange(bool isEdit);
};
}

Q_DECLARE_METATYPE(WaifuL2d::SceneControllerState)