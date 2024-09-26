#pragma once
#include <QGraphicsObject>
#include <CDT.h>

namespace WaifuL2d {
class OperatePoint;

class MeshEditor : public QGraphicsObject {
  Q_OBJECT

  QList<OperatePoint*> points;

  CDT::EdgeUSet fixedEdges;
  CDT::EdgeUSet allEdges;

  QRectF handleRect;

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

public:
  MeshEditor(const QList<QPointF>& initPoints,
             const QList<unsigned int>& initIncident,
             QGraphicsItem* parent = nullptr);
  MeshEditor(QGraphicsItem* parent = nullptr);
  void setHandleRect(const QRectF& rect);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
};
}