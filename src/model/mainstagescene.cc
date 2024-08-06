//
// Created by chenyichen on 8/4/24.
//

#include "mainstagescene.h"

#include "mesh.h"
#include "spriterendergroup.h"
namespace ProjectModel {
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
void MainStageScene::setUpGL() {}
}  // namespace ProjectModel
