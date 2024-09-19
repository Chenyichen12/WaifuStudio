#pragma once
#include <QGraphicsObject>

#include "deformercommand.h"
namespace WaifuL2d {
class Layer;
class AbstractDeformer;
class DeformManager : public QGraphicsObject {
  Q_OBJECT
  QList<AbstractDeformer*> deformers;
  int smallSize = 10;
 public:
  DeformManager();
  void addDeformer(AbstractDeformer* deformer);
  void removeDeformer(AbstractDeformer* deformer);
  void clearSelection();
  /**
   * select deformers from layer
   * will clear the selection first
   */
  void selectFromLayers(const QList<Layer*>& layers);
  AbstractDeformer* getDeformerFromLayer(const Layer* layer) const;
  int type() const override { return UserType + 1; }
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override {}

  /**
   * may emit the should select signal
   */
  void handleSelectClick(const QPointF& scenePos, bool isMulti);
  /**
   * get the selected deformers
   */
  QList<AbstractDeformer*> getSelectedDeformers() const;
  void emitDeformCommand(QSharedPointer<DeformerCommand> command);

  void setSmallSize(int size);
 signals:
  void deformCommand(QSharedPointer<DeformerCommand> command);
  void deformShouldSelect(QList<AbstractDeformer*> deformers);
};
}  // namespace WaifuL2d