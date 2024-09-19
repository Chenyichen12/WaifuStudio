#pragma once
#include <QList>
#include <QMetaType>
#include <QPointF>
#include <QUndoCommand>
namespace WaifuL2d {
class AbstractDeformer;
class DeformerCommand {
 protected:
  AbstractDeformer* targetMopher;

 public:
  QList<QPointF> oldPoints;
  QList<QPointF> newPoints;
  qreal oldAngle = 0;
  qreal newAngle = 0;
  enum DeformerUndoId { PointCommand = 1, AngleCommand = 2 };
  virtual QUndoCommand* createUndoCommand(QUndoCommand* parent = nullptr) = 0;
  explicit DeformerCommand(AbstractDeformer* mopher);
  ~DeformerCommand() = default;
};

class DeformerPointCommand : public DeformerCommand {
  bool isEnd;

 public:
  class PointUndoCommand : public QUndoCommand {
    QList<QPointF> oldPoints;
    QList<QPointF> newPoints;
    bool isEnd;
    AbstractDeformer* deformer;

   public:
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand* other) override;
    int id() const override;
    PointUndoCommand(AbstractDeformer* deformer,const QList<QPointF>& oldPoints,
                     const QList<QPointF>& newPoints, bool isEnd,
                     QUndoCommand* parent = nullptr);
  };
  DeformerPointCommand(AbstractDeformer* deformer, bool isEnd = false);
  QUndoCommand* createUndoCommand(QUndoCommand* parent = nullptr) override;
};
}  // namespace WaifuL2d

// Q_DECLARE_METATYPE(WaifuL2d::MopherCommand)