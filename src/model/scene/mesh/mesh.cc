#include "mesh.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include "../meshmathtool.hpp"
#include "rendergroup.h"
namespace WaifuL2d {
Mesh::Mesh(const QList<MeshVertex>& vertices,
           const QList<unsigned int>& incident)
    : vertices(vertices), incident(incident) {
  this->vao = new QOpenGLVertexArrayObject();
  this->vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  this->ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  this->tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
}

void Mesh::init(QOpenGLFunctions* f) {
  vao->create();
  vao->bind();

  vbo->create();
  vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
  vbo->bind();
  vbo->allocate(vertices.data(), vertices.size() * sizeof(MeshVertex));

  ibo->create();
  ibo->bind();
  ibo->allocate(incident.data(), incident.size() * sizeof(unsigned int));

  f->glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(MeshVertex),
                           (void*)MeshVertexOffset::Position);
  f->glEnableVertexAttribArray(0);
  f->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(MeshVertex),
                           (void*)MeshVertexOffset::UV);
  f->glEnableVertexAttribArray(1);
  vao->release();

  tex->create();
  tex->setData(meshImage);
}

bool Mesh::hitTest(const QPointF& pos) {
  // test in triangle
  MeshVertex p = {glm::vec2(pos.x(), pos.y()), glm::vec2(0, 0)};
  std::optional<std::array<MeshVertex, 3>> result;
  for (int i = 0; i < incident.size(); i += 3) {
    auto p0 = vertices[incident[i]];
    auto p1 = vertices[incident[i + 1]];
    auto p2 = vertices[incident[i + 2]];
    if (MeshMathTool<MeshVertex>::isInTriangle(p, p0, p1, p2)) {
      result = {p0, p1, p2};
      break;
    }
  }
  if (result == std::nullopt) {
    return false;
  }

  // test png alpha
  auto co = MeshMathTool<MeshVertex>::barycentricCoordinates(
      p, result.value()[0], result.value()[1], result.value()[2]);

  auto uv = MeshMathTool<MeshVertex>::fromBarycentricCoordinates(
      co, result.value()[0], result.value()[1], result.value()[2]);
  int row = uv.second * meshImage.height();
  int col = uv.first * meshImage.width();
  auto color = meshImage.pixelColor(col, row);
  if (color.alpha() <= 10) {
    return false;
  }
  return true;
}

void Mesh::render(QOpenGLFunctions* f, QOpenGLShaderProgram* program) {
  if (!visible) {
    return;
  }
  vao->bind();
  tex->bind(0);
  program->setUniformValue("ourTexture", 0);
  f->glDrawElements(GL_TRIANGLES, incident.size(), GL_UNSIGNED_INT, nullptr);
  tex->release();
  vao->release();
}
void Mesh::setTexture(const QImage& image) { meshImage = image; }
QList<QPointF> Mesh::getPos() {
  QList<QPointF> pos;
  for (auto& vertex : vertices) {
    pos.append(QPointF(vertex.x(), vertex.y()));
  }
  return pos;
}
QList<unsigned int> Mesh::getIncident() const { return this->incident; }

Mesh::~Mesh() {
  delete vao;
  delete vbo;
  delete ibo;
  delete tex;
}
void Mesh::changeVertexPos(const QPointF& pos, int index) {
  Q_ASSERT(index < vertices.size() && index >= 0);
  vertices[index].pos = {pos.x(), pos.y()};
}
void Mesh::upDateBuffer() {
  vbo->bind();
  vbo->write(0, vertices.data(), vertices.size() * sizeof(MeshVertex));
  vbo->release();
  if (container) {
#ifdef Q_OS_MACOS
    container->update();
#endif
    container->update();
  }
}
void Mesh::setVisible(bool vis) {
  visible = vis;
  if (container) {
    container->update();
  }
}

}  // namespace WaifuL2d
