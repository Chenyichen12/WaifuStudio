#pragma once
#include <QGraphicsScene>
namespace WaifuL2d {
class RenderGroup;
class DeformerCommand;
class AbstractDeformer;
class RootDeformer;
class MainStageScene : public QGraphicsScene {
  Q_OBJECT
 private:
  RenderGroup* renderGroup = nullptr;
  QGraphicsRectItem* backGroundItem;
  RootDeformer* rootDeformer;
  struct {
    bool pressed = false;
    bool moved = false;
    void clear() {
      pressed = false;
      moved = false;
    }
  } mouseState;

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 public:
  explicit MainStageScene(QObject* parent = nullptr);

  void initGL();
  ~MainStageScene() override;
  void setRenderGroup(RenderGroup* renderGroup);
  void addDeformer(AbstractDeformer* deformer,
                   AbstractDeformer* parent = nullptr);
  void setDeformerVisible(int id, bool visible);
  void selectDeformersById(const QList<int>& id);
  AbstractDeformer* findDeformerById(int id);
  void emitDeformerCommand(std::shared_ptr<DeformerCommand> command);
 signals:
  void deformerCommand(std::shared_ptr<DeformerCommand> command);
  void shouldSelectDeformers(QList<int> deformers);
};
}  // namespace WaifuL2d