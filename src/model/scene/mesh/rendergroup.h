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

  bool isGlInit = false;

public:
  void initGL();
  void addMesh(Mesh* mesh);
  explicit RenderGroup(const QRectF& projectRect,
                       QGraphicsItem* parent = nullptr);
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = nullptr) override;

  void reorderMesh();
  QRectF boundingRect() const override;
  ~RenderGroup() override;
};
} // namespace WaifuL2d