//
// Created by chenyichen on 8/4/24.
//

#pragma once
#include <QGraphicsScene>
#include <QOpenGLFunctions>
namespace Scene {

class Sprite;
class MeshRenderGroup;
class RootController;
class MainStageScene : public QGraphicsScene, protected QOpenGLFunctions {
  Q_OBJECT
 private:
  QGraphicsRectItem* boundRect;

  MeshRenderGroup* renderGroup;
  RootController* controllerRoot;
 public:
  explicit MainStageScene(MeshRenderGroup* renderGroup,
                          RootController* controllerGroup,
                          QObject* parent = nullptr);

 public slots:
  void initGL();

 protected:
  void drawBackground(QPainter* painter, const QRectF& rect) override;
};
}  // namespace ProjectModel
