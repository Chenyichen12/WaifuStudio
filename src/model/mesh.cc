//
// Created by chenyichen on 8/7/24.
//

#include "mesh.h"

#include "QOpenGLBuffer"
#include "QOpenGLVertexArrayObject"
#include "QPainter"
#include "QWidget"
#include "spriterendergroup.h"
namespace ProjectModel {

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

QRectF calculateBoundRect(std::vector<MeshVertex> vector) {
  float left = FLT_MAX;
  float top = FLT_MAX;
  float right = FLT_MIN;
  float button = FLT_MIN;
  for (const auto& item : vector) {
    if (item.pos.x < left) {
      left = item.pos.x;
    }
    if (item.pos.x > right) {
      right = item.pos.x;
    }
    if (item.pos.y < top) {
      top = item.pos.y;
    }
    if (item.pos.y > button) {
      button = item.pos.y;
    }
  }

  return QRectF(QPointF(left, top), QPointF(right, button));
}
Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> incident,
           QRect relativeRect, SpriteRenderGroup* parent)
    : QGraphicsItem(parent), vertices(vertices), incident(incident) {
  initializeOpenGLFunctions();
  this->boundRect = calculateBoundRect(vertices);
  auto normalize =
      normalization(vertices, relativeRect.width(), relativeRect.height());

  this->vao = new QOpenGLVertexArrayObject();
  vao->create();
  vao->bind();

  this->vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  vbo->create();
  vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
  vbo->bind();
  vbo->allocate(normalize.get(), vertices.size() * sizeof(MeshVertex));

  this->ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
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
}
QRectF Mesh::boundingRect() const { return this->boundRect; }
void Mesh::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                 QWidget* widget) {
  Q_UNUSED(option)
  if (this->parentItem() == nullptr) {
    return;
  }
  painter->beginNativePainting();
  auto parent = static_cast<SpriteRenderGroup*>(this->parentItem());
  auto totalPoint =
      QLineF(0, 0, parent->getRenderWidth(), parent->getRenderHeight());
  totalPoint = painter->transform().map(totalPoint);

  parent->getProgram()->bind();
  glViewport(painter->transform().dx(),
             widget->height() - painter->transform().dy() - totalPoint.dy(),
             totalPoint.dx(), totalPoint.dy());
  vao->bind();
  glDrawElements(GL_TRIANGLES, incident.size(), GL_UNSIGNED_INT, nullptr);
  vao->release();
  painter->endNativePainting();
}
Mesh::~Mesh() {
  delete vao;
  delete vbo;
  delete ibo;
}

void MeshBuilder::setUpDefault(const BitmapAsset* bitmap) {
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
Mesh* MeshBuilder::extractMesh() {
  if (this->bitmapImage == nullptr || this->projectRect == std::nullopt) {
    return nullptr;
  }
  auto s = new Mesh(this->vertices, this->verticesIndex, projectRect.value());
  return s;
}
void MeshBuilder::setUpProjectRect(QRect project) {
  this->projectRect = project;
}
}  // namespace ProjectModel
