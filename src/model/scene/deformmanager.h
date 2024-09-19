#pragma once
#include <QGraphicsObject>

#include "mophercommand.h"
namespace WaifuL2d {
class Layer;
class AbstractDeformer;
class DeformManager : public QGraphicsObject {
  Q_OBJECT
  QList<AbstractDeformer*> deformers;

 public:
  DeformManager();
  void addDeformer(AbstractDeformer* mopher);
  void removeDeformer(AbstractDeformer* mopher);
  void clearSelection();
  void selectFromLayers(const QList<Layer*>& layers);
  AbstractDeformer* getDeformerFromLayer(Layer* layer);
  int type() const override { return UserType + 1; }
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override {}

 signals:
  void deformCommand(QSharedPointer<MopherCommand> command);
};
}  // namespace WaifuL2d