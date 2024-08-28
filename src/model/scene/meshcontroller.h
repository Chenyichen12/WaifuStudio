#pragma once
#include "scenecontroller.h"

/**
 * the normal controller to control the mesh, you should not control mesh
 * without MeshController controller should not have any children so its local
 * point only make scene in keyframe module when the keyframe module going to
 * set position of the vertex it will call the function in local point
 */
namespace Scene {
class Mesh;
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

  void setActiveSelectController(ActiveSelectController controller) override;

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
}
