#pragma once
#include <QGraphicsItem>
namespace Scene {

class Mesh;

enum ControllerType {
  RootControllerType = QGraphicsItem::UserType + 1,
  MeshControllerType = QGraphicsItem::UserType + 2,
};

class AbstractController : public QGraphicsItem {
 public:
  virtual int controllerId() = 0;
  int type() const override = 0;
  virtual QPointF localPointToScene(const QPointF& point) = 0;
  virtual QPointF scenePointToLocal(const QPointF& point) = 0;
  AbstractController(AbstractController* parent = nullptr)
      : QGraphicsItem(parent) {}
};

/**
 * the root controller witch is a rectangle with project width and height
 * should be added into the scene
 * all controllers local position should >0 and <1 expect the point outside the project
 */
class RootController : public AbstractController {
 private:
  int width;
  int height;

 public:
  RootController(int width, int height);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  int controllerId() override;
  int type() const override;
  QPointF localPointToScene(const QPointF& point) override;
  QPointF scenePointToLocal(const QPointF& point) override;
};

/**
 * the normal controller to control the mesh, you should not control mesh without MeshController
 * controller should not have any children so its local point only make scene in keyframe module
 * when the keyframe module going to set position of the vertex it will call the function in local point
 */
class MeshController : public AbstractController {
 private:
  Mesh* controlMesh;

 public:
  MeshController(Mesh* controlMesh, AbstractController* parent);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  int controllerId() override;
  int type() const override;
  QPointF localPointToScene(const QPointF& point) override;
  QPointF scenePointToLocal(const QPointF& point) override;
};
}  // namespace Scene
