#pragma once

#include <QGraphicsItem>
#include <QList>
#include <QPointF>
class QUndoCommand;
namespace WaifuL2d {
class Layer;
class MopherManager;

class AbstractMopher : public QGraphicsItem {
 private:
  const Layer* bindLayer = nullptr;

 protected:
  AbstractMopher* morpherParent;
  QList<AbstractMopher*> morpherChildren;
  MopherManager* getManager();

 public:
  enum Type {
    ManagerType = UserType + 1,
    MeshMopherType = UserType + 2,
    RectMopherType = UserType + 3,
    RotationMopherType = UserType + 4,
  };
  virtual QList<QPointF> getScenePoints() = 0;
  virtual void setScenePoint(const QPointF& point, int index) = 0;
  virtual void setScenePoints(const QList<QPointF>& points) = 0;
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;
  virtual int type() const override = 0;
  virtual void setMopherParent(AbstractMopher* parent);
  AbstractMopher(QGraphicsItem* parent = nullptr);

  void setBindLayer(Layer* layer) { bindLayer = layer; }
  const Layer* getBindLayer() const { return bindLayer; }
};
}  // namespace WaifuL2d