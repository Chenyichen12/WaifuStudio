
/**
 * source file
 * scenecontroller.cc
 * meshcontroller.cc
 * rectselectcontroller.cc
 */

#pragma once
#include <QGraphicsItem>
class QUndoCommand;
class QUndoStack;

namespace Scene {

class Mesh;

enum ControllerType {
  RootControllerType = QGraphicsItem::UserType + 1,
  MeshControllerType = QGraphicsItem::UserType + 2,
};

class AbstractController : public QGraphicsItem {
 protected:
  /**
   * the parent and children is different from graphics items parent and
   * children its parent is represented to project controller parent, its
   * relative position is from controllerParent all controller all controllers'
   * GraphicsItemParent should be a root controller
   */
  AbstractController* controllerParent = nullptr;
  std::vector<AbstractController*> controllerChildren;
  bool selectAble = true;

 public:
  virtual int controllerId() = 0;
  int type() const override = 0;
  virtual QPointF localPointToScene(const QPointF& point) = 0;
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;
  /**
   * controller always make up by point
   * the basic change controller way is set point position
   * you won't need to implement when the controller is not editable
   * @param index the index of the point, it is specified of the controller data struct
   * @param scenePosition the position of the scene position, the next is the position of local
   */
  virtual void setPointFromScene(int index, const QPointF& scenePosition) {}
  virtual void setPointFromLocal(int index, const QPointF& localPosition) {}

  /**
   * selectable && visible in controller is the selected state
   */
  bool isControllerSelected() const { return this->isVisible() && selectAble; }
  bool isControllerSelectAble() const { return selectAble; }

  /**
   * control this controller can select
   * if the controller is not selectable, it will also hide the layer
   * @param isSelected
   */
  void setControllerSelectAble(bool isSelected);
  /**
   * if the layer is selectable, it should be visible
   * if the layer is unselectable, it should ignore
   */
  virtual void selectTheController();
  /**
   * same as setVisibility(false)
   */
  virtual void unSelectTheController() { this->setVisible(false); }

  AbstractController(QGraphicsItem* parent = nullptr) : QGraphicsItem(parent) {
    // controller default is invisible
    this->setVisible(false);
  }

  virtual void setControllerParent(AbstractController* controller);
  /**
   * call select the rect to select controller point
   * it will be called by scene when it is visible.
   * override to handle select point
   * @param sceneRect scene position of the selection rect
   */
  virtual void selectAtScene(QRectF sceneRect) {}

  const std::vector<AbstractController*>& getControllerChildren() const {
    return controllerChildren;
  }
  AbstractController* getControllerParent() const { return controllerParent; }
};

/**
 * one special controller not has controller id, just a helper controller to
 * help control the point
 */
class RectSelectController : public QGraphicsItem {
 private:
  QRectF boundRect;

  double padding = 10;
  double lineWidth = 5;

  // the start point and end point of once movement
  QPointF startPoint;
  QPointF endPoint;

  QPointF lastMovePoint;
  /**
   * test the rect hit, default the rect will be hit at border
   * @param p scene hit point
   * @return
   */
  virtual bool ifHitRectBorder(const QPointF& p) const;

 public:
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  /**
   * get the bound from the list of point from scene
   * @param list scene point list
   * @return bound rect, not contain padding
   */
  static QRectF boundRectFromPoints(const std::vector<QPointF>& list);

  /**
   * the appearance information of rect
   * also affect the event hit test
   * position from scene
   */
  void setBoundRect(const QRectF& rect);
  void setPadding(double padding);
  void setLineWidth(double lineWidth);

  RectSelectController(QGraphicsItem* parent = nullptr);

  /**
   * call the function when the rect is moving
   * get the delta by  p2 - p1
   * point position is in scene position
   */
  std::function<void(const QPointF&, const QPointF&)> moveCallBack =
      [](const auto& a, const auto& b) {};
  /**
   * call the function when the rect move end
   * the first point pargma is the start position
   * the second point pargma is the end position
   */
  std::function<void(const QPointF&, const QPointF&)> moveEndCallBack = [](const QPointF&,const QPointF&) {};
  /**
   * normally the rect will auto update position in move event
   * set it false to update manually update the rect select position
   */
  bool ifAutoMoveUpdate = true;
 protected:
  /**
   * when the mouse hit the item rect select controller will grab the event
   */
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

/**
 * the root controller witch is a rectangle with project width and height
 * should be added into the scene
 * all controllers local position should >0 and <1 expect the point outside the
 * project
 */
class RootController : public AbstractController {
 private:
  int width;
  int height;
  QUndoStack* controllerUndoStack = nullptr;

 public:
  RootController(int width, int height);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  int controllerId() override;
  int type() const override;

  typedef bool forEachCallBack(AbstractController* controller);
  void forEachController(const std::function<forEachCallBack>& callback) const;

  std::vector<AbstractController*> getSelectedChildren() const;

  QPointF localPointToScene(const QPointF& point) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  /**
   * the root controller manager the controller's undo command
   * the controller command should push to root
   * @param stack 
   */
  void setUndoStack(QUndoStack* stack);
  void pushUndoCommand(QUndoCommand* command);
};

/**
 * the normal controller to control the mesh, you should not control mesh
 * without MeshController controller should not have any children so its local
 * point only make scene in keyframe module when the keyframe module going to
 * set position of the vertex it will call the function in local point
 */
class MeshController : public AbstractController {
  class MeshControllerEventHandler;
  friend MeshControllerEventHandler;

 private:
  Mesh* controlMesh;
  std::vector<bool> selectedPoint;
  MeshControllerEventHandler* handler;
  RectSelectController* selectRectItem;

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  // QVariant itemChange(GraphicsItemChange change,
  //                     const QVariant& value) override;

  std::vector<QPointF> getSelectedPointScenePosition() const;

 public:
  MeshController(Mesh* controlMesh, QGraphicsItem* parent = nullptr);
  ~MeshController() override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  int controllerId() override;
  int type() const override;

  /**
   * use to control the mesh point, the basic control function
   * you should also call upDateMeshBuffer after set all the mesh control point
   * to update the opengl buffer of the actual mesh
   * @param index mesh point index
   * @param scenePosition scene position
   */
  void setPointFromScene(int index, const QPointF& scenePosition) override;
  void setPointFromLocal(int index, const QPointF& localPosition) override;

  QPointF localPointToScene(const QPointF& point) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  void unSelectPoint();
  /**
   * select the mesh point
   * will auto update the scene appearance
   * @param index
   */
  void selectPoint(int index);
  void upDateMeshBuffer() const;
  void selectAtScene(QRectF sceneRect) override;

  void unSelectTheController() override;
};

}  // namespace Scene
