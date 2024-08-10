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
  bool handle = true; // whether the item handle event

 public:
  /**
   * @param renderGroup which is the actual gl group witch render mesh of the
   * character
   * @param controllerGroup the controller root.
   * its children are the actual controller can render controller and control
   * mesh
   */
  explicit MainStageScene(MeshRenderGroup* renderGroup,
                          RootController* controllerGroup,
                          QObject* parent = nullptr);

 public slots:
  /**
   * when the gl is prepared should call init. otherwise you won't see the
   * texture
   */
  void initGL();

 protected:
  void drawBackground(QPainter* painter, const QRectF& rect) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
};
}  // namespace Scene
