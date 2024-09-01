#pragma once
#include "controllercommand.h"

namespace Scene {
class EditMeshController;
}

namespace Command {
struct EditMeshRecord;

/**
 * edit mesh controller undo command
 * the edit mesh has some different feature to normal controller
 * so create a new command
 */
class EditMeshControllerCommand : public QUndoCommand {
 private:
  bool first = true;
  Scene::EditMeshController* controller;
  // the record of the edit mesh controller
  std::unique_ptr<EditMeshRecord> oldRecord;
  std::unique_ptr<EditMeshRecord> newRecord;
 public:
  EditMeshControllerCommand(Scene::EditMeshController* controller,
                            QUndoCommand* parent = nullptr);
  void redo() override;
  void undo() override;

  /**
   * record the edit mesh
   * it will record the point, fixed edge and all edge
   */
  void recordOldState();
  void recordNewState();
  ~EditMeshControllerCommand() override;

  bool canDo() const;
};
}  // namespace Command