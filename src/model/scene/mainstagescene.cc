#include "mainstagescene.h"

#include "deformmanager.h"
#include "mesh/rendergroup.h"
namespace WaifuL2d {
struct MouseState {
  bool isMove = false;
  bool isPress = false;
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
  mouseState->isPress = true;
}
void MainStageScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  if (mouseState->isPress) {
    mouseState->isMove = true;
  }
}
void MainStageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (!mouseState->isMove) {
    // click handler select item
  }
  mouseState->isMove = false;
  mouseState->isPress = false;
}
MainStageScene::~MainStageScene() = default;
}  // namespace WaifuL2d