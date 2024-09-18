#pragma once
#include <QGraphicsScene>
namespace WaifuL2d {
class RenderGroup;
class MainStageScene : public QGraphicsScene {
 private:
  RenderGroup* renderGroup = nullptr;
  QGraphicsRectItem* backGroundItem;

 public:
  explicit MainStageScene(QObject* parent = nullptr);
  void setRenderGroup(RenderGroup* renderGroup);
  RenderGroup* getRenderGroup() const { return renderGroup; }
};
}  // namespace WaifuL2d