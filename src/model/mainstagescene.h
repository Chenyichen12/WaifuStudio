//
// Created by chenyichen on 8/4/24.
//

#pragma once
#include <QGraphicsScene>
namespace ProjectModel {

class Sprite;

class MainStageScene : public QGraphicsScene {
  Q_OBJECT
 private:
  int projectWidth;
  int projectHeight;
  QGraphicsItemGroup* spriteGroup;
  QGraphicsRectItem* backgroundRect;
  QGraphicsRectItem* sceneRect;

  QList<Sprite*> projectImageList;

 protected:
  void setUpSprite();

 public:
  explicit MainStageScene(int width, int height, QList<Sprite*> = {},
                          QObject* parent = nullptr);

 public slots:
  void setUpGL();
};
}  // namespace ProjectModel
