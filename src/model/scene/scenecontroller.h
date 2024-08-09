#pragma once
#include <QGraphicsItem>
namespace Scene {

class Mesh;

enum ControllerType {
  MeshControllerType = QGraphicsItem::UserType + 1,
};

class AbstractController : public QGraphicsItem {
 public:
  virtual int controllerId() = 0;
  int type() const override = 0;
  AbstractController(QGraphicsItem* parent = nullptr) : QGraphicsItem(parent) {}
};

class MeshController : public AbstractController {
 private:
  Mesh* controlMesh;

 public:
  MeshController(Mesh* controlMesh, QGraphicsItem* parent = nullptr);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  int controllerId() override;
  int type() const override;
};
}  // namespace Scene
