//
// Created by chenyichen on 8/4/24.
//

#include "mainstagescene.h"

#include <QPainter>
#include "mesh.h"
#include "spriterendergroup.h"
namespace Scene {
MainStageScene::MainStageScene(int width, int height, SpriteRenderGroup* group,
                               QObject* parent)
    : QGraphicsScene(parent) {

  auto backRect = QRectF(0, 0, width, height);
  auto bound = backRect.marginsAdded(
      QMarginsF(width / 2, height / 2, width / 2, height / 2));

  boundRect = new QGraphicsRectItem(bound);
  this->addItem(boundRect);
  renderGroup = group;
  this->addItem(renderGroup);
}

void MainStageScene::initGL() {
  initializeOpenGLFunctions();
  renderGroup->initializeGL();
}

void MainStageScene::drawBackground(QPainter* painter, const QRectF& rect) {
  Q_UNUSED(rect)

  painter->beginNativePainting();
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  painter->endNativePainting();
  QGraphicsScene::drawBackground(painter, rect);
}
}  // namespace ProjectModel
