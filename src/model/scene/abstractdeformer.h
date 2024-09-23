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
  AbstractDeformer* deformerParent = nullptr;
  QList<AbstractDeformer*> deformerChildren;

public:
#define TYPE_ENUM_LIST              \
  X(RootType, UserType + 1)         \
  X(MeshDeformerType, UserType + 2) \
  X(RectDeformerType, UserType + 3) \
  X(RotationDeformerType, UserType + 4)

  // 定义enum
  enum Type {
#define X(EnumName, EnumValue) EnumName = EnumValue,
    TYPE_ENUM_LIST
#undef X
  };

  static bool isDeformer(int type) {
    switch (type) {
#define X(EnumName, EnumValue) \
  case EnumValue:              \
    return true;
      TYPE_ENUM_LIST
#undef X
      default:
        return false;
    }
  }
#undef TYPE_ENUM_LIST

  virtual QList<QPointF> getScenePoints() const = 0;
  virtual void setScenePoints(const QList<QPointF>& points) = 0;
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;

  virtual void setDeformerSelect(bool select) { setVisible(select); };
  virtual bool isDeformerSelected() const { return isVisible(); }

  virtual bool isHitDeformer(const QPointF& point) const {
    return boundingRect().contains(point);
  }

  int type() const override = 0;
  virtual void setDeformerParent(AbstractDeformer* parent);

  QList<AbstractDeformer*> getDeformerChildren() const {
    return deformerChildren;
  }

  explicit AbstractDeformer(QGraphicsItem* parent = nullptr);

  void setBindId(int id) { this->id = id; }
  int getBindId() const { return id; }
};
} // namespace WaifuL2d