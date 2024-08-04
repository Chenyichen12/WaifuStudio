//
// Created by chenyichen on 8/4/24.
//

#pragma once
#include <QGraphicsScene>
namespace ProjectModel {

class MainStageScene : public QGraphicsScene {
 private:
  QGraphicsItemGroup* spriteGroup;
  QGraphicsRectItem* backgroundRect;

  QGraphicsRectItem* sceneRect;
 public:
  explicit MainStageScene(QObject* parent = nullptr);

};
}  // namespace ProjectModel
