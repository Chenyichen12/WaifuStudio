
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



enum ControllerType {
  RootControllerType = QGraphicsItem::UserType + 1,
  MeshControllerType = QGraphicsItem::UserType + 2,
  EditMeshControllerType = QGraphicsItem::UserType + 3, // editmeshcontroller.h
};

/**
 * the controller enum from the toolbar
 */
enum ActiveSelectController {
  RectController = 0,
  RotationController = 1,
  PenController = 2,
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
  virtual std::vector<QPointF> getPointFromScene() { return {}; };
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

  virtual void setActiveSelectController(ActiveSelectController controller) {}
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
  ActiveSelectController activeSelectController = RectController;
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
  void setActiveSelectController(ActiveSelectController controller) override;

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



}  // namespace Scene
