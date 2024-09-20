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
  int id = -1;

 protected:
  AbstractDeformer* deformerParent;
  QList<AbstractDeformer*> deformerChildren;

 public:
  enum Type {
    RootType = UserType + 1,
    MeshDeformerType = UserType + 2,
    RectDeformerType = UserType + 3,
    RotationDeformerType = UserType + 4,
  };
  virtual QList<QPointF> getScenePoints() const = 0;
  virtual void setScenePoints(const QList<QPointF>& points) = 0;
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;

  virtual void setDeformerSelect(bool select) { setVisible(select); };
  virtual bool isDeformerSelected() const { return isVisible(); }
  int type() const override = 0;
  virtual void setDeformerParent(AbstractDeformer* parent);
  QList<AbstractDeformer*> getDeformerChildren() const {
    return deformerChildren;
  }
  explicit AbstractDeformer(QGraphicsItem* parent = nullptr);

  void setBindId(int id) { this->id = id; }
  int getBindId() const { return id; }
};
}  // namespace WaifuL2d