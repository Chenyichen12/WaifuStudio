#include "rendergroup.h"

#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QWidget>

#include "mesh.h"
namespace WaifuL2d {
void RenderGroup::initGL() {
  if (isGLIintialized) {
    return;
  } else {
    isGLIintialized = true;
  }
  this->initializeOpenGLFunctions();
  program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                   ":/shader/bitmapshow.vert");
  program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                   ":/shader/bitmapshow.frag");
  program->link();

  for (auto& mesh : meshes) {
    mesh->init(this);
  }
}

void RenderGroup::addMesh(Mesh* mesh) {
  meshes.append(mesh);
  if (isGLIintialized) {
    mesh->init(this);
  }
}

RenderGroup::RenderGroup(const QRectF& projectRect, QGraphicsItem* parent)
    : projectRect(projectRect), QGraphicsObject(parent) {
  setFlag(QGraphicsItem::ItemIsSelectable, false);
  program = new QOpenGLShaderProgram();
}

// void RenderGroup::setVisible(Mesh* mesh, bool visible) {
//   mesh->setVisible(visible);
//   update();
// }

QRectF RenderGroup::boundingRect() const { return projectRect; }

RenderGroup::~RenderGroup() {
  for (auto& mesh : meshes) {
    delete mesh;
  }
  delete program;
}
void RenderGroup::paint(QPainter* painter,
                        const QStyleOptionGraphicsItem* option,
                        QWidget* widget) {
  if (!isGLIintialized) {
    return;
  }
  painter->beginNativePainting();
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  painter->endNativePainting();

  // draw a background rect
  auto brush = QBrush(Qt::gray);
  painter->setBrush(brush);
  painter->drawRect(projectRect);
  painter->beginNativePainting();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  program->bind();

  // set uniform
  float halfX = widget->width() / 2;
  float halfY = widget->height() / 2;
  float traMat[] = {
      static_cast<float>((painter->transform().m11()) / halfX),
      0.0f,
      static_cast<float>((painter->transform().dx() - halfX) / halfX),

      0.0f,
      static_cast<float>(-painter->transform().m11() / halfY),
      static_cast<float>((halfY - painter->transform().dy()) / halfY),

      0.0f,
      0.0f,
      1.0f};
  auto mat = QMatrix3x3(traMat);
  program->setUniformValue("uProjectionMatrix", mat);
  for (auto& mesh : meshes) {
    mesh->render(this, program);
  }
  program->release();
  painter->endNativePainting();
}
}  // namespace WaifuL2d
