//
// Created by chenyichen on 8/4/24.
//

#include "mainstagescene.h"

#include <QGraphicsEllipseItem>

#include "sprite.h"
namespace ProjectModel {
MainStageScene::MainStageScene(int width, int height,
                               QList<Sprite *> spriteList, QObject *parent)
    : QGraphicsScene(parent) {
  this->projectWidth = width;
  this->projectHeight = height;
  this->spriteGroup = new QGraphicsItemGroup();
  auto backRect = QRectF(0, 0, width, height);
  this->backgroundRect = new QGraphicsRectItem(backRect);
  this->projectImageList = spriteList;

  auto bound = backRect.marginsAdded(
      QMarginsF(width / 2, height / 2, width / 2, height / 2));
  this->sceneRect = new QGraphicsRectItem(bound);

  sceneRect->setBrush(QBrush(Qt::white));
  backgroundRect->setBrush(QBrush(Qt::green));

  this->addItem(sceneRect);
  this->addItem(spriteGroup);
  spriteGroup->addToGroup(backgroundRect);
  setUpSprite();
}
void MainStageScene::setUpSprite() {
  for (const auto &item : this->projectImageList) {
    this->spriteGroup->addToGroup(item);
  }
}
void MainStageScene::setUpGL() {
  for (const auto &item : this->projectImageList) {
    item->upDateBuffer(projectWidth, projectHeight);
  }
}
}  // namespace ProjectModel
