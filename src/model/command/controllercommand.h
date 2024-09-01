#pragma once
#include <QPointF>
#include <QUndoCommand>
namespace Scene {
class AbstractController;
class MeshController;
}  // namespace Scene

namespace Command {
struct ControllerCommandInfo {
  QPointF p;
  int index;
};

class ControllerCommand : public QUndoCommand {
 protected:
  Scene::AbstractController* controller;
  QList<ControllerCommandInfo> oldVertex;
  QList<ControllerCommandInfo> newVertex;

 public:
  ControllerCommand(Scene::AbstractController* controller,
                    QUndoCommand* parent = nullptr);

  /**
   * the function to record point
   * should add record before add to undo stack
   * @param info point info
   */
  virtual void addOldInfo(const ControllerCommandInfo& info);
  virtual void addNewInfo(const ControllerCommandInfo& info);
  void undo() override;
  void redo() override;
};

class MeshControllerCommand : public ControllerCommand {
private:
  bool first = true;
 public:
  MeshControllerCommand(Scene::MeshController* controller,
                        QUndoCommand* parent = nullptr);
  void undo() override;

  /**
   * normally the the QUndoCommand will call redo after push to the stack
   * redoNotFirst will be called when the command is not the first one in the stack
   */
  virtual void redoNotFirst();
  void redo() override;
  Scene::MeshController* controller_;
};
}  // namespace Command