#include "mainstagescene.h"

#include <QGraphicsSceneMouseEvent>
#include <QStack>
#include <QTime>

#include "abstractdeformer.h"
#include "mesh/rendergroup.h"
namespace WaifuL2d {

class RootDeformer : public AbstractDeformer {
 public:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = nullptr) override {}
  QRectF boundingRect() const override { return QRectF(); }
  QList<QPointF> getScenePoints() const override { return QList<QPointF>(); }
  void setScenePoints(const QList<QPointF>& points) override {}
  QPointF scenePointToLocal(const QPointF& point) override { return point; }
  int type() const override { return RootType; }
};
MainStageScene::MainStageScene(QObject* parent) : QGraphicsScene(parent) {
  backGroundItem = new QGraphicsRectItem();
  backGroundItem->setBrush(Qt::transparent);
  backGroundItem->setZValue(0);
  this->addItem(backGroundItem);
  this->rootDeformer = new RootDeformer();
  rootDeformer->setZValue(2);
  this->addItem(rootDeformer);
}

void MainStageScene::setRenderGroup(RenderGroup* renderGroup) {
  if (this->renderGroup) {
    delete this->renderGroup;
  }
  this->renderGroup = renderGroup;
  this->addItem(renderGroup);
  renderGroup->setZValue(1);
  auto boundR = renderGroup->boundingRect();
  auto mar = boundR.width() / 2;
  backGroundItem->setRect(boundR.marginsAdded({mar, mar, mar, mar}));
}

void MainStageScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsScene::mousePressEvent(event);
}
void MainStageScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsScene::mouseMoveEvent(event);
}
void MainStageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsScene::mouseReleaseEvent(event);
}
MainStageScene::~MainStageScene() = default;

void MainStageScene::emitDeformerCommand(
    std::shared_ptr<DeformerCommand> command) {
  emit deformerCommand(command);
}

void MainStageScene::addDeformer(WaifuL2d::AbstractDeformer* deformer,
                                 WaifuL2d::AbstractDeformer* parent) {
  if (parent == nullptr) {
    parent = rootDeformer;
  }
  deformer->setDeformerParent(parent);
  deformer->setParentItem(rootDeformer);
}

void MainStageScene::initGL() {
  if (renderGroup) {
    renderGroup->initGL();
  }
}

AbstractDeformer* MainStageScene::findDeformerById(int id) {
  auto findStack = QStack<AbstractDeformer*>({rootDeformer});
  while (!findStack.isEmpty()) {
    auto deformer = findStack.pop();
    if (deformer->getBindId() == id) {
      return deformer;
    }
    for (auto child : deformer->getDeformerChildren()) {
      findStack.push(child);
    }
  }
  return nullptr;
}
void MainStageScene::setDeformerVisible(int id, bool visible) {
  auto deformer = findDeformerById(id);
  if (deformer) {
    deformer->setEnabled(visible);
    deformer->setDeformerSelect(visible);
  }
}
}  // namespace WaifuL2d