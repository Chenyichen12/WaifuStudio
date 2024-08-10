//
// Created by chenyichen on 8/7/24.
//

#include "mesh.h"

#include "meshrendergroup.h"
#include "QOpenGLBuffer"
#include "QOpenGLVertexArrayObject"
#include "QPainter"
#include "QWidget"
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

/**
 * use to calculate Graphics item bound
 * @param vector scene position
 * @return bound
 */
QRectF calculateBoundRect(const std::vector<MeshVertex>& vector) {
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
  this->boundRect = calculateBoundRect(vertices);
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
  glViewport(painter->transform().dx(),
             widget->height() - painter->transform().dy() - totalPoint.dy(),
             totalPoint.dx(), totalPoint.dy());
  vao->bind();
  tex->bind(0);
  parent->getProgram()->setUniformValue("ourTexture", 0);

  glDrawElements(GL_TRIANGLES, incident.size(), GL_UNSIGNED_INT, nullptr);
  vao->release();
  painter->endNativePainting();
}

void Mesh::upDateBuffer() {
  //TODO: update opengl buffer

  qDebug() << "opengl buffer update start";
}

void Mesh::setTexture(const QImage& image) { this->image = image; }

Mesh::~Mesh() {
  vao->destroy();
  vbo->destroy();
  ibo->destroy();
  tex->destroy();
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
