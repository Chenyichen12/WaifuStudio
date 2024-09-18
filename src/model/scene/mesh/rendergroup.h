#pragma once
#include <QGraphicsObject>
#include <QList>
#include <QOpenGLFunctions>
class QOpenGLShaderProgram;
namespace WaifuL2d {
class Mesh;
class RenderGroup : public QGraphicsObject, public QOpenGLFunctions {
 private:
  QList<Mesh*> meshes;
  QRectF projectRect;
  QOpenGLShaderProgram* program;

  bool isGLIintialized = false;

 public:
  void initGL();
  void addMesh(Mesh* mesh);
  RenderGroup(const QRectF& projectRect, QGraphicsItem* parent = nullptr);
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  // void setVisible(Mesh* mesh, bool visible);
  QRectF boundingRect() const override;
  ~RenderGroup() override;
};
}  // namespace WaifuL2d