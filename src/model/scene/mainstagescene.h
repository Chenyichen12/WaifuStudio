#pragma once
#include <QDebug>
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

  /**
   * the scene has gl context
   * init the gl when gl is ready
   */
  void initGL();
  ~MainStageScene() override;

  /**
   * render group, it is the actual mesh render.
   * will take the ownership
   * @param renderGroup render group
   */
  void setRenderGroup(RenderGroup* renderGroup);

  /**
   * add the deformer to the scene
   * it will take the ownership
   * the z value will be set to top default
   * @param deformer deformer
   * @param parent deformer parent
   */
  void addDeformer(AbstractDeformer* deformer,
                   AbstractDeformer* parent = nullptr);

  /**
   * if the deformer is invisible it means the deformer won't appear and won't be selected some deformer will hide its children automatically
   * @param id 
   * @param visible 
   */
  void setDeformerVisible(int id, bool visible);
  /**
   * select the deformer
   * will automatically unselect other deformers
   * @param id 
   */
  void selectDeformersById(const QList<int>& id);

  AbstractDeformer* findDeformerById(int id);
  /**
   * call deformerCommand
   * @param command 
   */
  void emitDeformerCommand(std::shared_ptr<DeformerCommand> command);
  QList<AbstractDeformer*> getSelectedDeformers() const;

  QRectF getProjectRect() const;
signals:
  /**
   * when the deformer should call the undo command, it will be emitted to the project to add undo command
   * @param command undo command
   */
  void deformerCommand(std::shared_ptr<DeformerCommand> command);
  /**
   * if the deformer is selected by mouse press event, it will be emitted to the project to change selection
   * @param deformers 
   */
  void shouldSelectDeformers(QList<int> deformers);
};
} // namespace WaifuL2d