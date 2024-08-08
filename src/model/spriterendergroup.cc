//
// Created by chenyichen on 8/6/24.
//

#include "spriterendergroup.h"

#include "QBrush"
#include "QFile"
#include "QPainter"
#include "mesh.h"
namespace ProjectModel {
SpriteRenderGroup::SpriteRenderGroup(int renderWidth, int renderHeight,
                                     QGraphicsItem *parent)
    : QGraphicsItemGroup(parent),
      renderWidth(renderWidth),
      renderHeight(renderHeight) {
  backGroundItem =
      new QGraphicsRectItem(QRectF(0, 0, renderWidth, renderHeight));
  addToGroup(backGroundItem);
  backGroundItem->setBrush(QBrush(Qt::gray));
  program = new QOpenGLShaderProgram();
}
SpriteRenderGroup::~SpriteRenderGroup() { program->deleteLater(); }
void SpriteRenderGroup::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
  QGraphicsItemGroup::paint(painter, option, widget);
}
int SpriteRenderGroup::getRenderWidth() const { return renderWidth; }
int SpriteRenderGroup::getRenderHeight() const { return renderHeight; }

void SpriteRenderGroup::initializeGL() {
  program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                   ":/shader/bitmapshow.vert");
  program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                   ":/shader/bitmapshow.frag");
  program->link();
  for (auto mesh : meshList) {
    mesh->initializeGL(QRect(0, 0, renderWidth, renderHeight));
  }
}

QOpenGLShaderProgram *SpriteRenderGroup::getProgram() const { return program; }
void SpriteRenderGroup::pushBackMesh(Mesh *mesh) {
  this->addToGroup(mesh);
  meshList.push_back(mesh);

  mesh->setZValue(meshList.size());
}
void SpriteRenderGroup::pushFrontMesh(Mesh *mesh) {
  this->addToGroup(mesh);
  meshList.push_front(mesh);
  for (int i = 0; i < meshList.size(); ++i) {
    auto item = meshList[i];
    item->setZValue(i);
  }
}

}  // namespace ProjectModel
