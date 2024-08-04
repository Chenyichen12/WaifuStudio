//
// Created by chenyichen on 8/4/24.
//

#include "mainstagescene.h"

#include <QGraphicsEllipseItem>

#include "sprite.h"

namespace ProjectModel {
MainStageScene::MainStageScene(QObject *parent) : QGraphicsScene(parent) {
  this->spriteGroup = new QGraphicsItemGroup();
  this->backgroundRect = new QGraphicsRectItem(QRectF(0,0,800,600));
  this->sceneRect = new QGraphicsRectItem(QRectF(-1000,-1000,2000,2000));

  sceneRect->setBrush(QBrush(Qt::white));
  backgroundRect->setBrush(QBrush(Qt::green));

  this->addItem(sceneRect);
  this->addItem(spriteGroup);
  spriteGroup->addToGroup(backgroundRect);

  auto s = new Sprite();
  spriteGroup->addToGroup(s);
}
}  // namespace ProjectModel
