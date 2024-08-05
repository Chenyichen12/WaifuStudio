//
// Created by chenyichen on 8/5/24.
//

#include "sprite.h"

#include <QOpenGLTexture>

#include "QOpenGLBuffer"
#include "QPainter"
#include "QStyleOptionGraphicsItem"
#include "layer_bitmap.h"
namespace ProjectModel {
Sprite::Sprite(QGraphicsItem *parent) : QGraphicsItem(parent) {
  this->initializeOpenGLFunctions();
}

QRectF Sprite::boundingRect() const { return this->boundRect; }
void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) {
  painter->beginNativePainting();
  painter->endNativePainting();
}

const std::vector<SpriteVertex> &Sprite::getVertices() const {
  return vertices;
}
const std::vector<unsigned int> &Sprite::getVerticesIndex() const {
  return verticesIndex;
}
void Sprite::upDateBuffer(int width, int height) {
  delete indexBuffer;
  delete vertexBuffer;

  auto transformBuffer = std::make_unique<SpriteVertex[]>(vertices.size());
  for (int i = 0; i < vertices.size(); ++i) {
    const auto &item = vertices[i];
    transformBuffer[i].position.x = item.position.x / width;
    transformBuffer[i].position.y = item.position.y / height;
    transformBuffer[i].uv = item.uv;
  }

  vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  vertexBuffer->create();
  vertexBuffer->write(0, transformBuffer.get(), vertices.size());
  vertexBuffer->release();

  indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  indexBuffer->create();
  indexBuffer->write(0, verticesIndex.data(), verticesIndex.size());
  indexBuffer->release();
}
void Sprite::setVertices(const std::vector<SpriteVertex> &vertices) {
  Sprite::vertices = vertices;

  float left = FLT_MAX;
  float top = FLT_MAX;
  float right = FLT_MIN;
  float button = FLT_MIN;
  for (const auto &item : vertices) {
    if (item.position.x < left) {
      left = item.position.x;
    }
    if (item.position.x > right) {
      right = item.position.x;
    }
    if (item.position.y < top) {
      top = item.position.y;
    }
    if (item.position.y > button) {
      button = item.position.y;
    }
  }

  this->boundRect = QRectF(QPointF(left, top), QPointF(right, button));
}
void Sprite::setVerticesIndex(const std::vector<unsigned int> &verticesIndex) {
  Sprite::verticesIndex = verticesIndex;
}
void Sprite::setTexture(QOpenGLTexture *texture) { Sprite::texture = texture; }
Sprite::~Sprite() {
  delete this->indexBuffer;
  delete this->vertexBuffer;
  delete this->texture;
}

void SpriteBuilder::setUpDefault(const BitmapAsset &bitmap) {
  auto left = bitmap.left;
  auto right = bitmap.left + bitmap.width;
  auto top = bitmap.top;
  auto button = bitmap.top + bitmap.height;

  auto v1 = SpriteVertex();
  v1.position.x = static_cast<float>(left);
  v1.position.y = static_cast<float>(top);
  v1.uv.x = 0;
  v1.uv.y = 0;

  auto v2 = SpriteVertex();
  v2.position.x = static_cast<float>(right);
  v2.position.y = static_cast<float>(top);
  v2.uv.x = 1;
  v2.uv.y = 0;

  auto v3 = SpriteVertex();
  v3.position.x = static_cast<float>(right);
  v3.position.y = static_cast<float>(button);
  v3.uv.x = 1;
  v3.uv.y = 1;

  auto v4 = SpriteVertex();
  v4.position.x = static_cast<float>(left);
  v4.position.y = static_cast<float>(button);
  v4.uv.x = 0;
  v4.uv.y = 1;

  this->vertices.push_back(v1);
  this->vertices.push_back(v2);
  this->vertices.push_back(v3);
  this->vertices.push_back(v4);

  auto image = QImage(bitmap.rawBytes, bitmap.width, bitmap.height,
                      QImage::Format_RGBA8888);
  texture.reset(new QOpenGLTexture(image));
}
Sprite *SpriteBuilder::extractSprite() {
  if (this->texture == nullptr) {
    return nullptr;
  }
  auto s = new Sprite();
  s->setVertices(this->vertices);
  s->setVerticesIndex(this->verticesIndex);
  s->setTexture(this->texture.release());
  return s;
}
}  // namespace ProjectModel
