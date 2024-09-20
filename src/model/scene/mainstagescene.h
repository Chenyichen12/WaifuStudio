#pragma once
#include <QGraphicsScene>
namespace WaifuL2d {
class RenderGroup;
class DeformerCommand;
class AbstractDeformer;
class MainStageScene : public QGraphicsScene {
  Q_OBJECT
 private:
  RenderGroup* renderGroup = nullptr;
  QGraphicsRectItem* backGroundItem;
  AbstractDeformer* rootDeformer;
 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 public:
  explicit MainStageScene(QObject* parent = nullptr);

  void initGL();
  ~MainStageScene() override;
  void setRenderGroup(RenderGroup* renderGroup);
  void addDeformer(AbstractDeformer* deformer, AbstractDeformer* parent = nullptr);
  void setDeformerVisible(int id,bool visible);
  AbstractDeformer* findDeformerById(int id);
  void emitDeformerCommand(std::shared_ptr<DeformerCommand> command);
 signals:
  void deformerCommand(std::shared_ptr<DeformerCommand> command);
};
}  // namespace WaifuL2d