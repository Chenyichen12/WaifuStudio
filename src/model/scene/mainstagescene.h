#pragma once
#include <QGraphicsScene>
namespace WaifuL2d {
class RenderGroup;
class DeformManager;

struct MouseState;
class MainStageScene : public QGraphicsScene {
 private:
  RenderGroup* renderGroup = nullptr;
  DeformManager* deformManager = nullptr;
  QGraphicsRectItem* backGroundItem;
  std::unique_ptr<MouseState> mouseState;
 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 public:
  explicit MainStageScene(QObject* parent = nullptr);
  ~MainStageScene() override;

  void setRenderGroup(RenderGroup* renderGroup);
  void setDeformManager(DeformManager* deformManager);
  RenderGroup* getRenderGroup() const { return renderGroup; }
  DeformManager* getDeformManager() const { return deformManager; }
};
}  // namespace WaifuL2d