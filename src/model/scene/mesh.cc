//
// Created by chenyichen on 8/7/24.
//

#include "mesh.h"

#include "QOpenGLBuffer"
#include "QOpenGLVertexArrayObject"
#include "QPainter"
#include "QWidget"
#include "meshmathtool.hpp"
#include "meshrendergroup.h"
namespace Scene {

/**
 * map the scene position to the gl position
 * @param vertices scene position vertices
 * @param width scene width
 * @param height scene height
 * @return map gl vertices
 */
std::unique_ptr<MeshVertex[]> normalization(
    const std::vector<MeshVertex>& vertices, int width, int height) {
  auto res = std::make_unique<MeshVertex[]>(vertices.size());
  float midX = width / 2.0f;
  float midY = height / 2.0f;
  for (int i = 0; i < vertices.size(); ++i) {
    const auto& item = vertices[i];
    float posX = item.pos.x;
    float poxY = height - item.pos.y;

    res[i].pos.x = (posX - midX) / midX;
    res[i].pos.y = (poxY - midY) / midY;
    res[i].uv = item.uv;
  }

  return res;
}


void Mesh::initializeGL(QRect relativeRect) {
  initializeOpenGLFunctions();
  auto normalize =
      normalization(vertices, relativeRect.width(), relativeRect.height());
  vao->create();
  vao->bind();

  vbo->create();
  vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
  vbo->bind();
  vbo->allocate(normalize.get(), vertices.size() * sizeof(MeshVertex));

  ibo->create();
  ibo->bind();
  ibo->allocate(incident.data(), incident.size() * sizeof(unsigned int));

  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(MeshVertex),
                        (void*)MeshVertexOffset::Position);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(MeshVertex),
                        (void*)MeshVertexOffset::UV);
  glEnableVertexAttribArray(1);
  vao->release();

  tex->create();
  tex->setData(image);
}

Mesh::Mesh(const std::vector<MeshVertex>& vertices,
           const std::vector<unsigned int>& incident, MeshRenderGroup* parent)
    : QGraphicsItem(parent), vertices(vertices), incident(incident) {
  this->boundRect = MeshMathTool<MeshVertex>::calculateBoundRect(vertices);
  this->vao = new QOpenGLVertexArrayObject();
  this->vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  this->ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  this->tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
}

QRectF Mesh::boundingRect() const { return this->boundRect; }
void Mesh::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                 QWidget* widget) {
  Q_UNUSED(option)
  if (this->parentItem() == nullptr) {
    return;
  }
  painter->beginNativePainting();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  auto parent = static_cast<MeshRenderGroup*>(this->parentItem());
  auto totalPoint =
      QLineF(0, 0, parent->getRenderWidth(), parent->getRenderHeight());
  totalPoint = painter->transform().map(totalPoint);

  parent->getProgram()->bind();
  // wtf
  glViewport(painter->transform().dx() * 1.5,
             (widget->height() - painter->transform().dy() - totalPoint.dy())*1.5,
             totalPoint.dx() * 1.5, totalPoint.dy() * 1.5);
  vao->bind();
  tex->bind(0);
  parent->getProgram()->setUniformValue("ourTexture", 0);

  glDrawElements(GL_TRIANGLES, incident.size(), GL_UNSIGNED_INT, nullptr);
  vao->release();
  painter->endNativePainting();
}

void Mesh::setVerticesAt(int index, const MeshVertex& vertex) {
  vertices[index] = vertex;
  this->boundRect = MeshMathTool<MeshVertex>::calculateBoundRect(vertices);
}

void Mesh::upDateBuffer() {
  auto par = static_cast<MeshRenderGroup*>(parentItem());
  auto normalize =
      normalization(vertices, par->getRenderWidth(), par->getRenderHeight());
  vbo->bind();
  vbo->write(0, normalize.get(), vertices.size() * sizeof(MeshVertex));
  vbo->release();
  update();
}

void Mesh::resetBuffer() {
  auto par = static_cast<MeshRenderGroup*>(parentItem());
  auto normalize =
      normalization(vertices, par->getRenderWidth(), par->getRenderHeight());
  vbo->bind();
  vbo->allocate(normalize.get(), vertices.size() * sizeof(MeshVertex));
  vbo->release();
  ibo->bind();
  ibo->allocate(incident.data(), incident.size() * sizeof(unsigned int));
  ibo->release();
  update();
}

void Mesh::setTexture(const QImage& image) { this->image = image; }

QImage Mesh::getTextureImage() const { return this->image; }

void Mesh::setMeshPointFromScene(const std::vector<QPointF>& points,
                                 const std::vector<unsigned int>& inc) {
  std::vector<MeshVertex> finalRes;
  const auto& indexes = MeshMathTool<MeshVertex>::calculateBoundIndex(vertices);
  for (const auto& point : points) {
    auto assignPoint = MeshVertex{{point.x(), point.y()}, {}};
    bool isComplete = false;
    for (int i = 0; i < incident.size(); i += 3) {
      auto&& p1 = vertices[incident[i]];
      auto&& p2 = vertices[incident[i + 1]];
      auto&& p3 = vertices[incident[i + 2]];
      if (MeshMathTool<MeshVertex>::isInTriangle(assignPoint, p1, p2, p3)) {
        auto&& info = MeshMathTool<MeshVertex>::barycentricCoordinates(
            assignPoint, p1, p2, p3);
        auto uvP1 = QPointF(p1.uv.x, p1.uv.y);
        auto uvP2 = QPointF(p2.uv.x, p2.uv.y);
        auto uvP3 = QPointF(p3.uv.x, p3.uv.y);

        auto uv = MeshMathTool<QPointF>::fromBarycentricCoordinates(info, uvP1,
                                                                    uvP2, uvP3);
        assignPoint.uv = {static_cast<float>(uv.x()),
                          static_cast<float>(uv.y())};
        isComplete = true;
        break;
      }
    }
    if (isComplete) {
      finalRes.push_back(assignPoint);
      continue;
    }
    // TODO: out of triangle handler
  }
  this->vertices = finalRes;
  this->incident = inc;
  update();
}

Mesh::~Mesh() {
  delete vao;
  delete vbo;
  delete ibo;
  delete tex;
}

void MeshBuilder::setUpDefault(const ProjectModel::BitmapAsset* bitmap) {
  auto left = bitmap->left;
  auto right = bitmap->left + bitmap->width;
  auto top = bitmap->top;
  auto button = bitmap->top + bitmap->height;

  auto v1 = MeshVertex();
  v1.pos.x = static_cast<float>(right);
  v1.pos.y = static_cast<float>(button);
  v1.uv.x = 1;
  v1.uv.y = 1;

  auto v2 = MeshVertex();
  v2.pos.x = static_cast<float>(left);
  v2.pos.y = static_cast<float>(button);
  v2.uv.x = 0;
  v2.uv.y = 1;

  auto v3 = MeshVertex();
  v3.pos.x = static_cast<float>(left);
  v3.pos.y = static_cast<float>(top);
  v3.uv.x = 0;
  v3.uv.y = 0;

  auto v4 = MeshVertex();
  v4.pos.x = static_cast<float>(right);
  v4.pos.y = static_cast<float>(top);
  v4.uv.x = 1;
  v4.uv.y = 0;

  this->vertices.push_back(v1);
  this->vertices.push_back(v2);
  this->vertices.push_back(v3);
  this->vertices.push_back(v4);

  verticesIndex.push_back(0);
  verticesIndex.push_back(1);
  verticesIndex.push_back(2);

  verticesIndex.push_back(0);
  verticesIndex.push_back(2);
  verticesIndex.push_back(3);

  this->bitmapImage.reset(new QImage(bitmap->rawBytes, bitmap->width,
                                     bitmap->height, QImage::Format_RGBA8888));
}

void MeshBuilder::setUpVertices(const std::vector<MeshVertex>& vertices,
                                const std::vector<unsigned int>& index) {
  this->vertices = vertices;
  this->verticesIndex = index;
}
std::unique_ptr<Mesh> MeshBuilder::buildMesh() {
  if (this->bitmapImage == nullptr) {
    return nullptr;
  }
  auto s = std::make_unique<Mesh>(vertices, verticesIndex);
  s->setTexture(*bitmapImage);
  return s;
}

void MeshBuilder::setUpTexture(const QImage& image) {
  this->bitmapImage.reset(new QImage(image));
}
}  // namespace Scene
