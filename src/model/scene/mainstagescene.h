#pragma once
#include <QGraphicsScene>
#include <QDebug>
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
    QPointF startPos;
    QPointF lastPos;
    void clear() {
      pressed = false;
      startPos = QPointF();
      lastPos = QPointF();
    }
    bool isMoved() const {
      auto delta = lastPos - startPos;
      return delta.manhattanLength() > 20;
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