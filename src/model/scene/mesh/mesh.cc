#include "mesh.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
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
}  // namespace WaifuL2d
