//
// Created by chenyichen on 8/4/24.
//

#pragma once
#include <QGraphicsScene>
#include <QOpenGLFunctions>
class QUndoStack;

namespace Scene {

class Sprite;
class MeshRenderGroup;
class RootController;

class MainStageEventHandler;
class MainStageScene : public QGraphicsScene, protected QOpenGLFunctions {
  Q_OBJECT
  friend MainStageEventHandler;

 private:
  QGraphicsRectItem* boundRect;

  MeshRenderGroup* renderGroup;
  RootController* controllerRoot;

 protected:
  MainStageEventHandler* handler;  // event handler

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
  ~MainStageScene() override;

  void setVisibleOfLayer(int controllerOrMeshId, bool visible);
 signals:
  void selectionChanged(const std::vector<int>& indexId);
 public slots:
  /**
   * when the gl is prepared should call init. otherwise you won't see the
   * texture
   */
  void initGL();
  /**
   * handle the event when the view use rubber to select point
   * @param sceneRect rect of the rubber
   */
  void handleRubberSelect(QRectF sceneRect);

  /**
   * view connect this slot when the user click at one point
   * @param scenePoint
   * @param isMultiple
   */
  void handleSelectClick(QPointF scenePoint, bool isMultiple);

  /**
   * emit when the scene select layer by itself
   * @param selectionId the id of the layer. normally from controllerLayerModel
   */
  void selectLayers(const std::vector<int>& selectionId);

  /**
   * set scene undo stack
   * it will push undo command some scene command
   */
  void setSceneUndoStack(QUndoStack* undoStack);
 protected:
  void drawBackground(QPainter* painter, const QRectF& rect) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
};
}  // namespace Scene
