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

void Mesh::setZValue(qreal z) {
  zValue = z;
  if (container) {
    container->reorderMesh();
  }
}

void Mesh::setMeshStruct(const QList<MeshVertex>& ver,
                         const QList<unsigned int>& incidents) {
  this->vertices = ver;
  this->incident = incidents;
  vbo->bind();
  vbo->allocate(vertices.data(), vertices.size() * sizeof(MeshVertex));
  ibo->bind();
  ibo->allocate(incidents.data(), incident.size() * sizeof(unsigned int));
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

QList<MeshVertex> Mesh::getVertices() const { return this->vertices; }

QList<unsigned int> Mesh::getIncident() const { return this->incident; }

QPointF Mesh::uvAtPoint(const QPointF& pos) const {
  // in triangle
  MeshVertex result = {{pos.x(), pos.y()}};

  for (int i = 0; i < incident.size(); i += 3) {
    auto p1 = this->vertices[incident[i]];
    auto p2 = this->vertices[incident[i + 1]];
    auto p3 = this->vertices[incident[i + 2]];
    if (MeshMathTool<MeshVertex>::isInTriangle(result, p1, p2, p3)) {
      auto cor =
          MeshMathTool<MeshVertex>::barycentricCoordinates(result, p1, p2, p3);
      auto uv = MeshMathTool<MeshVertex>::fromBarycentricCoordinates(cor, p1,
        p2, p3);
      return {uv.first, uv.second};
    }
  }

  auto boundIndex = MeshMathTool<MeshVertex>::boundPointIndex(vertices.data(),
    vertices.size());

  auto topV = vertices[boundIndex[2]];
  auto bottomV = vertices[boundIndex[3]];
  qreal vRa = (bottomV.v() - topV.v()) / (bottomV.y() - topV.y());
  auto resultV = bottomV.v() - (bottomV.y() - pos.y()) * vRa;

  auto leftV = vertices[boundIndex[0]];
  auto rightV = vertices[boundIndex[1]];
  qreal uRa = (rightV.u() - leftV.u()) / (rightV.x() - leftV.x());
  auto resultU = rightV.u() - (rightV.x() - pos.x()) * uRa;

  return {resultU, resultV};
}

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
} // namespace WaifuL2d