//
// Created by chenyichen on 8/7/24.
//
#pragma once

#include "QGraphicsItem"
#include "QOpenGLFunctions"
#include "QOpenGLShaderProgram"
#include "QOpenGLTexture"
#include "glm/glm.hpp"
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

namespace ProjectModel {
class BitmapAsset;
class SpriteRenderGroup;

struct MeshVertex {
  glm::vec2 pos;
  glm::vec2 uv;
};
enum MeshVertexOffset {
  Position = offsetof(MeshVertex, pos),
  UV = offsetof(MeshVertex, uv)
};
class Mesh : public QGraphicsItem, protected QOpenGLFunctions {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> incident;
  QRectF boundRect;
  int layerId = 0;

 private:
  // gl data struct
  QOpenGLVertexArrayObject* vao;
  QOpenGLBuffer* vbo;
  QOpenGLBuffer* ibo;

 public:
  Mesh(std::vector<MeshVertex>, std::vector<unsigned int>, QRect relativeRect,
       SpriteRenderGroup* parent = nullptr);
  ~Mesh() override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  void bindId(int id) { layerId = id; }
};

class MeshBuilder {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> verticesIndex;
  std::unique_ptr<QImage> bitmapImage;
  std::optional<QRect> projectRect;

 public:
  void setUpDefault(const BitmapAsset& bitmap);
  void setUpProjectRect(QRect project);
  Mesh* extractMesh();
};
}  // namespace ProjectModel
