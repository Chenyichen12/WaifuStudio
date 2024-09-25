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

  /**
   * every deformer is made up with points
   * get the points positions
   * @return points position
   */
  virtual QList<QPointF> getScenePoints() const = 0;
  /**
   * set point positions
   * @param points positions in scene coordinate
   */
  virtual void setScenePoints(const QList<QPointF>& points) = 0;

  /**
   * useful for key point module
   * some deformer may record the local points of its children
   * so it will change the scene points to its local coordinate
   * @param point scene points
   * @return local points
   */
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;

  /**
   * deformer select is different from graphics item select
   * should call it when select or unselect of a deformer
   * @param select 
   */
  virtual void setDeformerSelect(bool select) { setVisible(select); };
  virtual bool isDeformerSelected() const { return isVisible(); }

  /**
   * when the deformer is unselected
   * if mouse press event hit the deformer, it will be selected default
   * @param point 
   * @return 
   */
  virtual bool isHitDeformer(const QPointF& point) const {
    return boundingRect().contains(point);
  }

  int type() const override = 0;
  virtual void setDeformerParent(AbstractDeformer* parent);

  QList<AbstractDeformer*> getDeformerChildren() const {
    return deformerChildren;
  }

  explicit AbstractDeformer(QGraphicsItem* parent = nullptr);

  /**
   * the bind id of one layer
   * a deformer will equal to a layer
   * and the id is the key for scene to find deformer
   * @param id layer id
   */
  void setBindId(int id) { this->id = id; }
  int getBindId() const { return id; }
};
} // namespace WaifuL2d