//
// Created by chenyichen on 8/7/24.
//
#pragma once

#include "../layer_bitmap.h"
#include "QGraphicsItem"
#include "QOpenGLFunctions"
#include "QOpenGLTexture"
#include "glm/glm.hpp"
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
namespace Scene {
class MeshRenderGroup;

struct MeshVertex {
  glm::vec2 pos;
  glm::vec2 uv;
};
enum MeshVertexOffset {
  Position = offsetof(MeshVertex, pos),
  UV = offsetof(MeshVertex, uv)
};
class Mesh : public QGraphicsItem, protected QOpenGLFunctions {
  friend MeshRenderGroup;

 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> incident;
  QRectF boundRect;
  int layerId = 0;
  QImage image;

 private:
  // gl data struct
  QOpenGLVertexArrayObject* vao;
  QOpenGLBuffer* vbo;
  QOpenGLBuffer* ibo;
  QOpenGLTexture* tex;

 protected:
  void initializeGL(QRect relativeRect);

 public:
  Mesh(const std::vector<MeshVertex>&, const std::vector<unsigned int>&,
       MeshRenderGroup* parent = nullptr);
  ~Mesh() override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  void setTexture(const QImage& image);
  void bindId(int id) { layerId = id; }
};

class MeshBuilder {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> verticesIndex;
  std::unique_ptr<QImage> bitmapImage;

 public:
  void setUpDefault(const ProjectModel::BitmapAsset* bitmap);
  void setUpVertices(const std::vector<MeshVertex>& vertices,
                     const std::vector<unsigned int>& index);
  void setUpTexture(const QImage& image);
  std::unique_ptr<Mesh> buildMesh();
};
}  // namespace Scene
