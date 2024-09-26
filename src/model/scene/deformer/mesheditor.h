#pragma once
#include <QGraphicsObject>
#include <CDT.h>

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
 * it can change the event handler during different edit tools and do cdt operation
 * this class is designed to edit the mesh
 */
class MeshEditor : public QGraphicsObject {
  Q_OBJECT

  QList<OperatePoint*> points;

  // this is the transparent area for editor to handle event, like add point
  // you can set it to invisible to disable the event handler
  QGraphicsRectItem* penSurface;

  CDT::EdgeUSet fixedEdges;
  CDT::EdgeUSet allEdges;


  OperatePoint* createPoint(const QPointF& pos);
  /**
   * update the points member
   * will add or delete points, auto manage the memory, auto set data of the operate point
   * but the index will fit the pointPositions param
   * should call it after point changed
   * @param pointPositions point positions
   */
  void updatePoints(const QList<QPointF>& pointPositions);

  void handlePointShouldMove(const QPointF& pos, bool isStart,
                             const QVariant& data);

  void handleShouldAddPoint(const QPointF& pos);

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

  void setPoints(const QList<QPointF>& points);
signals:
  void editorCommand(std::shared_ptr<MeshEditorCommand> command);
};
}