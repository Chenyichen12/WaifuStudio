#include "mainstagescene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTime>

#include "deformmanager.h"
#include "mesh/rendergroup.h"
namespace WaifuL2d {
struct MouseState {
  QTime startTime;
};
MainStageScene::MainStageScene(QObject* parent) : QGraphicsScene(parent) {
  backGroundItem = new QGraphicsRectItem();
  backGroundItem->setBrush(Qt::transparent);
  this->addItem(backGroundItem);
  mouseState = std::make_unique<MouseState>();
}

void MainStageScene::setRenderGroup(RenderGroup* renderGroup) {
  if (this->renderGroup) {
    removeItem(renderGroup);
    delete this->renderGroup;
  }
  this->renderGroup = renderGroup;
  this->addItem(renderGroup);
  renderGroup->setZValue(1);
  auto boundR = renderGroup->boundingRect();
  auto mar = boundR.width() / 2;
  backGroundItem->setRect(boundR.marginsAdded({mar, mar, mar, mar}));
}
void MainStageScene::setDeformManager(DeformManager* deformManager) {
  this->deformManager = deformManager;
  this->addItem(deformManager);
  deformManager->setZValue(2);
}
void MainStageScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  mouseState->startTime = QTime::currentTime();
  QGraphicsScene::mousePressEvent(event);
}
void MainStageScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsScene::mouseMoveEvent(event);
}
void MainStageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  auto time = mouseState->startTime.msecsTo(QTime::currentTime());
  if (time < 100) {
    deformManager->handleSelectClick(event->scenePos(),
                                     event->modifiers() == Qt::ShiftModifier);
  }
  QGraphicsScene::mouseReleaseEvent(event);
}
MainStageScene::~MainStageScene() = default;
}  // namespace WaifuL2d