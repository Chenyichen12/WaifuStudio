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
             QWidget* widget = nullptr) override {
  }

  QRectF boundingRect() const override { return QRectF(); }
  QList<QPointF> getScenePoints() const override { return QList<QPointF>(); }

  void setScenePoints(const QList<QPointF>& points) override {
  }

  QPointF scenePointToLocal(const QPointF& point) const override { return point; }
  int type() const override { return RootType; }

  typedef std::function<bool(AbstractDeformer*)> callBack;

  void forEachDown(const callBack& call) const {
    auto callStack = QStack<AbstractDeformer*>();
    for (const auto& deformer : deformerChildren) {
      callStack.push(deformer);
    }
    while (!callStack.isEmpty()) {
      auto deformer = callStack.pop();
      if (call(deformer)) {
        return;
      }
      for (const auto& child : deformer->getDeformerChildren()) {
        callStack.push(child);
      }
    }
  }
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
  mouseState.pressed = true;
  mouseState.startPos = event->screenPos();
  mouseState.lastPos = event->screenPos();
}

void MainStageScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsScene::mouseMoveEvent(event);
  if (mouseState.pressed) {
    mouseState.lastPos = event->screenPos();
  }
}

void MainStageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  QGraphicsScene::mouseReleaseEvent(event);
  if (event->isAccepted()) {
    return;
  }
  if (mouseState.isMoved()) {
    mouseState.clear();
    return;
  } else {
    mouseState.clear();
  };
  auto posItems = items(event->scenePos());
  std::reverse(posItems.begin(), posItems.end());
  auto deformers = QList<AbstractDeformer*>();
  bool isMulti = event->modifiers() == Qt::ControlModifier;
  for (const auto& item : posItems) {
    if (AbstractDeformer::isDeformer(item->type())) {
      deformers.push_back(static_cast<AbstractDeformer*>(item));
    }
  }

  AbstractDeformer* readyDeformer = nullptr;
  AbstractDeformer* existDeformer = nullptr;
  for (const auto& item : deformers) {
    if (item->isDeformerSelected()) {
      existDeformer = item;
      break;
    }
    if (item->isHitDeformer(event->scenePos())) {
      readyDeformer = item;
    }
  }

  QList<int> readySelectLayers;
  if (existDeformer) {
    readySelectLayers.append(existDeformer->getBindId());
  }
  rootDeformer->forEachDown([&readySelectLayers, isMulti](AbstractDeformer* d) {
    if (isMulti) {
      if (d->isDeformerSelected()) {
        readySelectLayers.append(d->getBindId());
      }
    }
    return false;
  });
  if (readyDeformer && !existDeformer) {
    readySelectLayers.append(readyDeformer->getBindId());
  }

  emit shouldSelectDeformers(readySelectLayers);
}

MainStageScene::~MainStageScene() = default;

void MainStageScene::selectDeformersById(const QList<int>& id) {
  rootDeformer->forEachDown([&id](AbstractDeformer* d) {
    d->setDeformerSelect(id.contains(d->getBindId()));
    return false;
  });
}

void MainStageScene::emitDeformerCommand(
    std::shared_ptr<DeformerCommand> command) {
  emit deformerCommand(command);
}

QList<AbstractDeformer*> MainStageScene::getSelectedDeformers() const {
  QList<AbstractDeformer*> result;
  rootDeformer->forEachDown([&result](AbstractDeformer* d) {
    if (d->isDeformerSelected()) {
      result.append(d);
    }
    return false;
  });
  return result;
}

QRectF MainStageScene::getProjectRect() const {
  return renderGroup->boundingRect();
}

void MainStageScene::focusDeformer(AbstractDeformer* deformer) {
  // need a controller, But now it seems no need
  if (deformer == nullptr) {
    this->rootDeformer->forEachDown([](AbstractDeformer* target) {
      target->setOpacity(1);
      return false;
    });
    update();
    return;
  }

  this->rootDeformer->forEachDown(
      [deformer](AbstractDeformer* target) {
        if (target != deformer) {
          target->setOpacity(0.2);
        }
        return false;
      });
  update();
}

void MainStageScene::addDeformer(WaifuL2d::AbstractDeformer* deformer,
                                 WaifuL2d::AbstractDeformer* parent) {
  if (parent == nullptr) {
    parent = rootDeformer;
  }
  deformer->setDeformerParent(parent);
  deformer->setParentItem(rootDeformer);
  auto size = rootDeformer->childItems().size();
  deformer->setZValue(size);
}

void MainStageScene::initGL() {
  if (renderGroup) {
    renderGroup->initGL();
  }
}

AbstractDeformer* MainStageScene::findDeformerById(int id) {
  AbstractDeformer* result = nullptr;
  rootDeformer->forEachDown([id, &result](AbstractDeformer* d) {
    if (d->getBindId() == id) {
      result = d;
      return true;
    }
    return false;
  });
  return result;
}

void MainStageScene::setDeformerVisible(int id, bool visible) {
  auto deformer = findDeformerById(id);
  if (deformer) {
    deformer->setVisible(visible);
  }
}
} // namespace WaifuL2d