
/**
 * source file
 * scenecontroller.cc
 * meshcontroller.cc
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
  EditMeshControllerType = QGraphicsItem::UserType + 3, // editmeshcontroller.h
};
enum ActiveSelectController {
  RectController = 0,
  RotationController = 1,
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
   * the get and set function for controller point
   * controller always make up by point
   * the basic change controller way is set point position
   * you won't need to implement when the controller is not editable
   * @param index the index of the point, it is specified of the controller data
   * struct
   * @param scenePosition the position of the scene position, the next is the
   * position of local
   */
  virtual void setPointFromScene(int index, const QPointF& scenePosition) {}
  virtual void setPointFromLocal(int index, const QPointF& localPosition) {}
  virtual std::vector<QPointF> getPointFromScene() { return {}; }
  virtual std::vector<QPointF> getPointFromLocal();

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

class EditMeshController;
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

  /**
   * change the select controller of all the controller.
   * affect the controller who has select controller
   * @param controller 
   */
  void setSelectController(ActiveSelectController controller);

  /**
   * call this to add editmeshcontroller
   * it will hide all controller expect the editmeshcontroller
   * @param controller 
   */
  void addEditMeshController(EditMeshController* controller);

  /**
   * to remove edit controller
   * will release the owner to the caller, will not release memory
   * @param controller 
   */
  void removeEditMeshController(EditMeshController* controller);
};

/**
 * the normal controller to control the mesh, you should not control mesh
 * without MeshController controller should not have any children so its local
 * point only make scene in keyframe module when the keyframe module going to
 * set position of the vertex it will call the function in local point
 */
class AbstractSelectController;
class MeshController : public AbstractController {
  class MeshControllerEventHandler;
  friend MeshControllerEventHandler;

 private:
  Mesh* controlMesh;
  std::vector<bool> selectedPoint;
  MeshControllerEventHandler* handler;

  int activeSelectController = 0;
  std::array<AbstractSelectController*, 2> selectControllerList;

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  std::vector<QPointF> getSelectedPointScenePosition() const;

 public:
  MeshController(Mesh* controlMesh, QGraphicsItem* parent = nullptr);
  ~MeshController() override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  int controllerId() override;
  int type() const override;



  void setActiveSelectController(ActiveSelectController controller);

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

  /**
   * select table
   * if the index of point is selected vector[index] == true
   * @return
   */
  const std::vector<bool>& getSelectedTable() const;
  /**
   * @return selectPointIndex list
   */
  std::vector<int> getSelectedPointIndex() const;

  /**
   * get the scene position of one point
   * has a better performance
   * @param index point index
   * @return position
   */
  QPointF getPointScenePosition(int index) const;

  std::vector<QPointF> getPointFromScene() override;
};

}  // namespace Scene
