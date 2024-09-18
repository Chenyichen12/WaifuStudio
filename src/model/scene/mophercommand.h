#pragma once
#include <QList>
#include <QMetaType>
#include <QPointF>
class QUndoCommand;
namespace WaifuL2d {
class AbstractMopher;
class MopherCommand {
  AbstractMopher* targetMopher;

 public:
  QList<QPointF> oldPoints;
  QList<QPointF> newPoints;
  qreal oldAngle = 0;
  qreal newAngle = 0;
  enum UndoId {
    PointCommand = 1,
    AngleCommand = 2
  };
  virtual QUndoCommand* createUndoCommand(QUndoCommand* parent = nullptr) = 0;
  explicit MopherCommand(AbstractMopher* mopher);
  ~MopherCommand() = default;
};
}  // namespace WaifuL2d

// Q_DECLARE_METATYPE(WaifuL2d::MopherCommand)