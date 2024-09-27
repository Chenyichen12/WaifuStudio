#pragma once
#include <CDT.h>

#include <QGraphicsObject>
#include "scenecontroller.h"

class QUndoCommand;

namespace WaifuL2d {
class OperatePoint;

class MeshEditorCommand {
public:
  virtual QUndoCommand* createUndoCommand(QUndoCommand* parent = nullptr) = 0;
};

/**
 * the editor is different from deformer
 * it should be created by scene controller
 * it can change the event handler during different edit tools and do cdt
 * operation this class is designed to edit the mesh
 */
class MeshEditor : public QGraphicsObject {
  Q_OBJECT

  QList<OperatePoint*> points;

  // this is the transparent area for editor to handle event, like add point
  // you can set it to invisible to disable the event handler
  std::array<QGraphicsRectItem*, 3> toolSurface;
  // QGraphicsRectItem* penSurface;

  CDT::EdgeUSet fixedEdges;
  CDT::EdgeUSet allEdges;

  OperatePoint* createPoint(const QPointF& pos);

  void handlePointShouldMove(const QPointF& pos, bool isStart,
                             const QVariant& data);

  void handleShouldAddPoint(const QPointF& pos);

  void handleShouldConnect(int index1, int index2);

protected:
  CDT::Triangulation<double> calculateCDT() const;
  QList<OperatePoint*> getSelectedPoint() const;

public:
  MeshEditor(const QList<QPointF>& initPoints,
             const QList<unsigned int>& initIncident,
             QGraphicsItem* parent = nullptr);
  MeshEditor(QGraphicsItem* parent = nullptr);
  void setHandleRect(const QRectF& rect);
  QList<QPointF> getPoints() const;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;


  void setEditTool(EditToolType type);

  /**
   * the operation will always success.
   * will make sure the new point index at the end of the point list
   * just like append(point)
   * @param point 
   */
  void addPoint(const QPointF& point);

  void addPoint(const QPointF& point, int index);

  /**
   * add fixed Edge to the editor
   * will always success expect the index out of range
   * @param index1 
   * @param index2 
   */
  void connectFixEdge(unsigned int index1, unsigned int index2);
  /**
   * remove the fixed edge of the editor
   * @param index1 
   * @param index2 
   */
  void disconnectFixEdge(unsigned int index1, unsigned int index2);

  /**
   * remove the point at the index
   * will always success expect the index out of range
   * just like erase(index)
   * will auto delete the edge of the point
   * @param index 
   */
  void removePoint(int index);
  /**
   * to solve some problem when point out of range
   * delete multiple points should call this function
   * @param indexes 
   */
  void removePoints(const QList<int>& indexes);

  /**
   * make sure the points size is equal to the new points size
   * just change the position of the points and update cdt
   * won't add or remove
   * @param points 
   */
  void updatePointsPos(const QList<QPointF>& points);
  /**
   * this is a dangerous function
   * through the new points size to auto decide delete or add or update points
   * But it not makes sure to success
   * @param points 
   */
  void setPoints(const QList<QPointF>& points);

  void selectPoints(const QList<int>& indexes);

signals:
  void editorCommand(std::shared_ptr<MeshEditorCommand> command);
};
} // namespace WaifuL2d