//
// Created by chenyichen on 8/6/24.
//
#pragma once
#include <QGraphicsItemGroup>
#include <QOpenGLShaderProgram>
namespace Scene {
class Mesh;
class MeshRenderGroup : public QGraphicsItemGroup {
 private:
  QGraphicsRectItem *backGroundItem;
  int renderWidth;
  int renderHeight;

  QOpenGLShaderProgram *program;

  QList<Mesh *> meshList;

 public:
  MeshRenderGroup(int renderWidth, int renderHeight,
                    QGraphicsItem *parent = nullptr);
  ~MeshRenderGroup() override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
  int getRenderWidth() const;
  int getRenderHeight() const;

  void setMeshVisible(int id, bool visible);

  void initializeGL();

  QOpenGLShaderProgram *getProgram() const;

  void pushBackMesh(Mesh* mesh);
  void pushFrontMesh(Mesh* mesh);

};
}  // namespace ProjectModel
