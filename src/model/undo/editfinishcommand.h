#pragma once
#include <QUndoCommand>
namespace WaifuL2d {
class MainStageScene;
class MeshDeformer;
class EditFinishCommand :public QUndoCommand {
  MainStageScene* scene;
  MeshDeformer* deformer;

  struct Data;
  std::unique_ptr<Data> d;

public:
  EditFinishCommand(MainStageScene* scene, MeshDeformer* deformer,
                    const QList<QPointF>& changePoints,
                    const QList<unsigned int>& incident,
                    QUndoCommand* parent = nullptr);
  ~EditFinishCommand() override;
  void redo() override;
  void undo() override;
};


class EditFinishCommandWrapper {
  MainStageScene* scene_ = nullptr;
  MeshDeformer* deformer_ = nullptr;
  QList<QPointF> changePoints;
  QList<unsigned int> incident;

public:
  EditFinishCommand* createCommand(QUndoCommand* parent = nullptr) const;

  void setTarget(MainStageScene* scene, MeshDeformer* deformer);
  void setData(const QList<QPointF>& changePoints,
               const QList<unsigned int>& incident);
};
}