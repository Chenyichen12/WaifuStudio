#pragma once

#include <QGraphicsItem>
#include <QList>
#include <QPointF>
class QUndoCommand;
namespace WaifuL2d {
class Layer;
class DeformManager;

class AbstractDeformer : public QGraphicsItem {
 private:
  const Layer* bindLayer = nullptr;

 protected:
  AbstractDeformer* deformerParent;
  QList<AbstractDeformer*> deformerChildren;
  DeformManager* getManager();

 public:
  enum Type {
    ManagerType = UserType + 1,
    MeshDeformerType = UserType + 2,
    RectDeformerType = UserType + 3,
    RotationDeformerType = UserType + 4,
  };
  virtual QList<QPointF> getScenePoints() const = 0;
  virtual void setScenePoint(const QPointF& point, int index) = 0;
  virtual void setScenePoints(const QList<QPointF>& points) = 0;
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;

  virtual void setSmallSize(int size){};

  virtual void setDeformerSelect(bool select) { setVisible(select); };
  virtual bool isDeformerSelected() const { return isVisible(); }
  int type() const override = 0;
  virtual void setDeformerParent(AbstractDeformer* parent);
  explicit AbstractDeformer(QGraphicsItem* parent = nullptr);

  void setBindLayer(const Layer* layer) { bindLayer = layer; }
  const Layer* getBindLayer() const { return bindLayer; }
};
}  // namespace WaifuL2d