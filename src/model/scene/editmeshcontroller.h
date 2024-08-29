#pragma once
#include "mesh.h"
#include "scenecontroller.h"
namespace Scene {
class PointEventHandler;
class AbstractSelectController;
class EditMeshController : public AbstractController {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> incident;
  PointEventHandler* pointHandler;
  std::vector<int> selectIndex;

  int activeSelectTool = 0;
  std::array<AbstractSelectController*, 3> selectControllerTool;

  void upDateActiveTool();
 protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
public:
  EditMeshController(const std::vector<MeshVertex>& vertices,
                     const std::vector<unsigned int>& incident,
                     QGraphicsItem* parent = nullptr);
  ~EditMeshController() override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  QRectF boundingRect() const override;
  int type() const override;
  // return -1 will not affect by layer model
  int controllerId() override { return -1; }
  QPointF localPointToScene(const QPointF& point) override;
  QPointF scenePointToLocal(const QPointF& point) override;
  std::vector<QPointF> getPointFromScene() override;

  /**
   * handle selection from the scene
   * @param sceneRect 
   */
  void selectAtScene(QRectF sceneRect) override;
  void setPointFromScene(int index, const QPointF& scenePosition) override;

  /**
   * select the point of the index
   * it will show the select controller if select point's size has 2 or more
   * @param index 
   */
  void selectPoint(int index);

  /**
   * to cancel the all the select
   */
  void unSelectPoint();

  /**
   * add undo command to the root
   * if no root will delete the command
   * @param command 
   */
  void addUndoCommand(QUndoCommand* command) const;

  void setActiveSelectController(ActiveSelectController controller) override;
  /**
   * get select point index
   * @return index
   */
  std::vector<int> getSelectIndex() const;
};
}  // namespace Scene
