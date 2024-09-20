#pragma once
#include <QList>
#include <QMetaType>
#include <QPointF>
#include <QUndoCommand>
namespace WaifuL2d {
class AbstractDeformer;

struct BasicDeformerCommandInfo {
  AbstractDeformer* target;
  QList<QPointF> oldPoints;
  QList<QPointF> newPoints;
  bool isEnd;
  qreal oldAngle = 0;
  qreal newAngle = 0;
};
class DeformerCommand {
 public:
  BasicDeformerCommandInfo info;
  enum DeformerUndoId { BasicMoveCommandId = 0 };

  explicit DeformerCommand();
  virtual QUndoCommand* createUndoCommand(QUndoCommand* parent = nullptr);

  class BasicMoveCommand : public QUndoCommand {
   private:
    BasicDeformerCommandInfo info;

   public:
    BasicMoveCommand(BasicDeformerCommandInfo info, QUndoCommand* parent);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand* other) override;
    int id() const override { return BasicMoveCommandId; }
  };
};
}  // namespace WaifuL2d

// Q_DECLARE_METATYPE(WaifuL2d::MopherCommand)