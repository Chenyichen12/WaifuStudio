//
// Created by chenyichen on 8/6/24.
//

#include "MeshRenderGroup.h"

#include "QFile"
#include "QPainter"
#include "mesh.h"
namespace Scene {
MeshRenderGroup::MeshRenderGroup(int renderWidth, int renderHeight,
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
MeshRenderGroup::~MeshRenderGroup() { program->deleteLater(); }
void MeshRenderGroup::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
  QGraphicsItemGroup::paint(painter, option, widget);
}
int MeshRenderGroup::getRenderWidth() const { return renderWidth; }
int MeshRenderGroup::getRenderHeight() const { return renderHeight; }

void MeshRenderGroup::initializeGL() {
  program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                   ":/shader/bitmapshow.vert");
  program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                   ":/shader/bitmapshow.frag");
  program->link();
  for (auto mesh : meshList) {
    mesh->initializeGL(QRect(0, 0, renderWidth, renderHeight));
  }
}

QOpenGLShaderProgram *MeshRenderGroup::getProgram() const { return program; }
void MeshRenderGroup::pushBackMesh(Mesh *mesh) {
  this->addToGroup(mesh);
  meshList.push_back(mesh);

  mesh->setZValue(meshList.size());
}
void MeshRenderGroup::pushFrontMesh(Mesh *mesh) {
  this->addToGroup(mesh);
  meshList.push_front(mesh);
  for (int i = 0; i < meshList.size(); ++i) {
    auto item = meshList[i];
    item->setZValue(i);
  }
}

}  // namespace ProjectModel
