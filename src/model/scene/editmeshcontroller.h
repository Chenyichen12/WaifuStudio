#pragma once
#include "mesh.h"
#include "scenecontroller.h"
namespace Scene {
class PointEventHandler;
class EditMeshController : public AbstractController {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> incident;
  PointEventHandler* pointHandler;
  std::vector<int> selectIndex;
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
  void selectAtScene(QRectF sceneRect) override;
  void setPointFromScene(int index, const QPointF& scenePosition) override;

  void selectPoint(int index);
  void unSelectPoint();
  
  void setActiveSelectController(ActiveSelectController controller) override;
};
}  // namespace Scene
