#pragma once
#include <QGraphicsObject>

#include "mophercommand.h"
namespace WaifuL2d {
class Layer;
class AbstractMopher;
class MopherManager : public QGraphicsObject {
  Q_OBJECT
  QList<AbstractMopher*> mophers;

 public:
  MopherManager();
  void addMopher(AbstractMopher* mopher);
  void removeMopher(AbstractMopher* mopher);
  void clearSelection();
  void selectFromLayers(const QList<Layer*>& layers);
  int type() const override { return UserType + 1; }

 signals:
  void mopherCommand(QSharedPointer<MopherCommand> command);
};
}  // namespace WaifuL2d