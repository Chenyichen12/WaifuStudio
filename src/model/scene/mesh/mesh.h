#pragma once
#include <QImage>
#include <QList>
#include <QPointF>
#include <glm/glm.hpp>
class QOpenGLFunctions;
class QOpenGLVertexArrayObject;
class QOpenGLBuffer;
class QOpenGLTexture;
class QOpenGLShaderProgram;
namespace WaifuL2d {
struct MeshVertex {
  glm::vec2 pos;
  glm::vec2 uv;
  float x() const { return pos.x; }
  float y() const { return pos.y; }
};
enum MeshVertexOffset {
  Position = offsetof(MeshVertex, pos),
  UV = offsetof(MeshVertex, uv)
};

class RenderGroup;
class Mesh {
  QList<MeshVertex> vertices;
  QList<unsigned int> incident;
  QImage meshImage;
  bool visible = true;

  QOpenGLVertexArrayObject* vao;
  QOpenGLBuffer* vbo;
  QOpenGLBuffer* ibo;
  QOpenGLTexture* tex;

  friend RenderGroup;
  RenderGroup* container = nullptr;

 public:
  Mesh(const QList<MeshVertex>& vertices, const QList<unsigned int>& incident);
  void init(QOpenGLFunctions* f);
  void render(QOpenGLFunctions* f, QOpenGLShaderProgram* program);
  void setTexture(const QImage& image);
  QList<QPointF> getPos();
  QList<unsigned int> getIncident() const;
  void setVisible(bool vis);

  void changeVertexPos(const QPointF& pos, int index);
  void upDateBuffer();

  ~Mesh();
};
}  // namespace WaifuL2d