#pragma once
#include <QGraphicsScene>
namespace WaifuL2d {
class RenderGroup;
class DeformManager;
class MainStageScene : public QGraphicsScene {
 private:
  RenderGroup* renderGroup = nullptr;
  DeformManager* deformManager = nullptr;
  QGraphicsRectItem* backGroundItem;
 public:
  explicit MainStageScene(QObject* parent = nullptr);
  void setRenderGroup(RenderGroup* renderGroup);
  void setDeformManager(DeformManager* deformManager);
  RenderGroup* getRenderGroup() const { return renderGroup; }
  DeformManager* getDeformManager() const { return deformManager; }
};
}  // namespace WaifuL2d