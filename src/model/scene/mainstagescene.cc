#include "mainstagescene.h"

#include "mesh/rendergroup.h"
namespace WaifuL2d {
MainStageScene::MainStageScene(QObject* parent) : QGraphicsScene(parent) {
  backGroundItem = new QGraphicsRectItem();
  backGroundItem->setBrush(Qt::transparent);
  this->addItem(backGroundItem);
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
}  // namespace WaifuL2d