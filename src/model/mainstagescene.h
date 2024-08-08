//
// Created by chenyichen on 8/4/24.
//

#pragma once
#include <QGraphicsScene>
#include <QOpenGLFunctions>
namespace ProjectModel {

class Sprite;
class SpriteRenderGroup;
class MainStageScene : public QGraphicsScene, protected QOpenGLFunctions {
  Q_OBJECT
 private:
  QGraphicsRectItem* boundRect;
  SpriteRenderGroup* renderGroup;

 public:
  explicit MainStageScene(int width, int height, SpriteRenderGroup* renderGroup,
                          QObject* parent = nullptr);

 public slots:
  void initGL();

 protected:
  void drawBackground(QPainter* painter, const QRectF& rect) override;
};
}  // namespace ProjectModel
