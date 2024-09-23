#pragma once
#include <QModelIndex>
#include <QUndoCommand>

#include "waifucommand.h"
namespace WaifuL2d {
class LayerModel;
class MainStageScene;
class VisibleCommand : public QUndoCommand {
 private:
  LayerModel* layerModel;
  QModelIndexList indexes;
  MainStageScene* scene;
  QList<bool> visibles;

  bool isStart = false;

 public:
  VisibleCommand(LayerModel* layerModel, MainStageScene* scene,
                 const QModelIndex& index, bool visible);
  void setStart(bool start);
  void undo() override;
  void redo() override;
  bool mergeWith(const QUndoCommand* command) override;
  int id() const override { return VisibleCommandId; }
};
}  // namespace WaifuL2d