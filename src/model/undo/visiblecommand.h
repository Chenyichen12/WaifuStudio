#pragma once
#include <QModelIndex>
#include <QUndoCommand>
#include "waifucommand.h"
namespace WaifuL2d {
class RenderGroup;
class LayerModel;
class DeformManager;
class VisibleCommand : public QUndoCommand {
 private:
  RenderGroup* renderGroup;
  LayerModel* layerModel;
  DeformManager* deformManager;
  QModelIndexList indexes;
  QList<bool> visibles;

  bool isStart = false;

 public:
  VisibleCommand(RenderGroup* renderGroup, LayerModel* layerModel,
                 DeformManager* deformManager, const QModelIndex& index,
                 bool visible);
  void setStart(bool start);
  void undo() override;
  void redo() override;
  bool mergeWith(const QUndoCommand* command) override;
  int id() const override { return VisibleCommandId; }
};
}  // namespace WaifuL2d