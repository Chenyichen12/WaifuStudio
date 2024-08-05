//
// Created by chenyichen on 8/5/24.
//
#pragma once
#include <QGraphicsItem>
#include <QOpenGLFunctions>
#include <glm/glm.hpp>
#include <vector>

#include "QOpenGLTexture"
class QOpenGLBuffer;
namespace ProjectModel {
class BitmapAsset;
struct SpriteVertex {
  glm::vec2 position;  // 顶点坐标
  glm::vec2 uv;        // UV 坐标

  // 构造函数
  SpriteVertex(const glm::vec2& pos, const glm::vec2& texCoord)
      : position(pos), uv(texCoord) {}

  SpriteVertex() = default;
};

class Sprite : public QGraphicsItem, protected QOpenGLFunctions {
 private:

  QOpenGLBuffer* vertexBuffer = nullptr;
  QOpenGLBuffer* indexBuffer = nullptr;
  QOpenGLTexture* texture = nullptr;

  std::vector<SpriteVertex> vertices;
  std::vector<unsigned int> verticesIndex;

  QRectF boundRect;

 public:
  explicit Sprite(QGraphicsItem* parent = nullptr);
  ~Sprite() override;
  QRectF boundingRect() const override;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  const std::vector<SpriteVertex>& getVertices() const;
  const std::vector<unsigned int>& getVerticesIndex() const;
  void setVertices(const std::vector<SpriteVertex>& vertices);
  void setVerticesIndex(const std::vector<unsigned int>& verticesIndex);
  void setTexture(QOpenGLTexture* texture);


  void upDateBuffer(int width, int height);
};

class SpriteBuilder {
 private:
  std::vector<SpriteVertex> vertices;
  std::vector<unsigned int> verticesIndex;
  std::unique_ptr<QOpenGLTexture> texture;

 public:
  void setUpDefault(const BitmapAsset& bitmap);
  Sprite* extractSprite();
};
}  // namespace ProjectModel
