#pragma once
#include "CDT.h"
#include "mesh.h"
#include "scenecontroller.h"
namespace Scene {
class PointEventHandler;
class AbstractSelectController;
class EditMeshController : public AbstractController {
 private:
  PointEventHandler* pointHandler;

  std::unordered_set<int> selectIndex;
  std::vector<CDT::V2d<float>> editPoint;  // the point in editMesh
  CDT::EdgeUSet fixedEdge;                 // the fixed Edge in CDT
  CDT::EdgeUSet selectedFixEdge;           // the select Edge in CDT. it should
                                           // be a sub set of fixedEdge

  CDT::EdgeUSet allEdge;  // all edge

  int activeSelectTool = 0;
  std::array<AbstractSelectController*, 3> selectControllerTool;

  void upDateActiveTool();

 protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

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
   * to get the edge information of the edit mesh
   * @return fixed edge
   */
  const CDT::EdgeUSet& getFixedEdge() const;
  /**
   * @return all edge
   */
  const CDT::EdgeUSet& getAllEdge() const;

  /**
   * set the all information of the edit point
   * normally used by undo command
   * it will unselect the select point because the index error
   * @param points edit points scene position
   * @param fixedEdge
   * @param allEdge
   */
  void setEditMesh(const std::vector<QPointF>& points,
                   const CDT::EdgeUSet& fixedEdge,
                   const CDT::EdgeUSet& allEdge);

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
   * to select edge
   * @param fixedEdge
   */
  void selectFixedEdge(const CDT::Edge& fixedEdge);
  /**
   * to cancel the all the select
   */
  void unSelectFixedEdge();

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

  /**
   * get selected fix edge of the edit mesh
   * @return selected edge
   */
  std::vector<CDT::Edge> getSelectedEdge() const;

  /**
   * transform incident to edge struct to do cdt
   * @return cdt edge
   */
  static CDT::EdgeUSet incidentToEdge(const std::vector<unsigned int>&);

  /**
   * add a point to edit mesh
   * won't auto call updateCDT call it if you need
   * @param scenePoint the point in scene
   * @param select if auto select the point
   */
  void addEditPoint(const QPointF& scenePoint, bool select = true);
  /**
   * add point to edit mesh and also add a fixed edge
   * between old point and new point
   * @param scenePoint new point in scene
   * @param lastSelectIndex the old point to be connected
   * @param select if auto select the point
   */
  void addEditPoint(const QPointF& scenePoint, int lastSelectIndex,
                    bool select = true);

  /**
   * connect the fix edge between two index
   * @param index1 normally the current select index
   * @param index2 normally the mouse pressed index
   * @param selectIndex2 if select the index2
   */
  void connectFixedEdge(int index1, int index2, bool selectIndex2 = true);
  /**
   * auto connect the point with no fixed edge
   * the actual CDT Algorithm will be called in this function
   * with the fixed edge and editPoint in this instance
   * the fixed edge may change when the fixed edge was cross each other
   * should upDateCDT to fit the allowable mesh
   */
  void upDateCDT();

  /**
   * remove the current index
   * @note it may change the index of some point, if you want to remove
   * multiPoint, should call removePoints
   * @param index the index of editPoint
   * @param withEdge if remove the point and its edge
   */
  void removePoint(int index, bool withEdge = true);
  void removePoints(std::vector<int> index, bool withEdge = true);

  /**
   * remove the fixed edge of the edit mesh
   * @param index1 point index
   * @param index2 point index
   */
  void removeFixedEdge(int index1, int index2);
  void removeFixedEdge(const CDT::Edge& edge);
};
}  // namespace Scene
