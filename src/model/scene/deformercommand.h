#pragma once
#include <QList>
#include <QMetaType>
#include <QPointF>
#include <QUndoCommand>
namespace WaifuL2d {
class AbstractDeformer;
class DeformerCommand : public QUndoCommand {
 protected:
  AbstractDeformer* targetDeformer;
  bool isEnd = false;

 public:
  QList<QPointF> oldPoints;
  QList<QPointF> newPoints;
  qreal oldAngle = 0;
  qreal newAngle = 0;

  enum DeformerUndoId { BasicMoveCommand = 1 };
  explicit DeformerCommand(AbstractDeformer* deformer,
                           QUndoCommand* parent = nullptr);
  DeformerCommand(const DeformerCommand& other, QUndoCommand* parent = nullptr);
  bool mergeWith(const QUndoCommand* other) override;
  void undo() override;
  void redo() override;
  int id() const override { return BasicMoveCommand; }
  void setEnd(bool end) { isEnd = end; }
  bool getEnd() const { return isEnd; }
};
}  // namespace WaifuL2d

// Q_DECLARE_METATYPE(WaifuL2d::MopherCommand)