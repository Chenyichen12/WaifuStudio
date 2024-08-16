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

  void addOldInfo(const ControllerCommandInfo& info);
  void addNewInfo(const ControllerCommandInfo& info);
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
  void redo() override;
  Scene::MeshController* controller_;
};
}  // namespace Command