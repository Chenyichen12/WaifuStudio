//
// Created by chenyichen on 8/4/24.
//

#include "mainstagescene.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include "mesh.h"
#include "meshrendergroup.h"
#include "scenecontroller.h"
#include "views/layertreeview.h"

namespace Scene {

class MainStageEventHandler {
 private:
  MainStageScene* context;

  bool handle = true;

 public:
  MainStageEventHandler(MainStageScene* scene) { context = scene; }

  void mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (handle) {
      context->QGraphicsScene::mousePressEvent(event);
    }
  }

  void keyPressEvent(QKeyEvent* event) {
    // when the space key pressed the view should be move
    // so the item shouldn't handle mouse event when view move
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
      handle = false;
    }
    context->QGraphicsScene::keyPressEvent(event);
  }
  void keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
      handle = true;
    }
    context->QGraphicsScene::keyReleaseEvent(event);
  }
};

void MainStageScene::setSceneMode(SceneMode mode) { this->sceneMode = mode; }

MainStageScene::SceneMode MainStageScene::getSceneMode() const {
  return this->sceneMode;
}

MainStageScene::MainStageScene(MeshRenderGroup* group,
                               RootController* controllerGroup, QObject* parent)
    : QGraphicsScene(parent) {
  int width = group->getRenderWidth();
  int height = group->getRenderHeight();
  auto backRect = QRectF(0, 0, width, height);
  auto bound = backRect.marginsAdded(
      QMarginsF(width / 2, height / 2, width / 2, height / 2));
  this->setItemIndexMethod(NoIndex);
  boundRect = new QGraphicsRectItem(bound);
  this->addItem(boundRect);
  renderGroup = group;
  this->controllerRoot = controllerGroup;
  this->addItem(renderGroup);
  this->addItem(controllerGroup);

  this->handler = new MainStageEventHandler(this);
}

MainStageScene::~MainStageScene() { delete handler; }

void MainStageScene::setVisibleOfLayer(int controllerOrMeshId, bool visible) {
  this->controllerRoot->forEachController([&](auto a) {
    if (a->controllerId() == controllerOrMeshId) {
      a->setControllerSelectAble(visible);
      return false;
    }
    return true;
  });
  this->renderGroup->setMeshVisible(controllerOrMeshId, visible);
}

MeshRenderGroup* MainStageScene::getRenderGroup() const { return renderGroup; }

RootController* MainStageScene::getControllerRoot() const {
  return controllerRoot;
}

void MainStageScene::initGL() {
  initializeOpenGLFunctions();
  renderGroup->initializeGL();
}

void MainStageScene::handleRubberSelect(QRectF sceneRect) {
  this->controllerRoot->forEachController([&](auto* controller) {
    if (controller->isVisible()) {
      controller->selectAtScene(sceneRect);
    }
    return true;
  });
}

/**
 * basic logic
 * click the point to find all the controller under this scene point
 * if one of the controller is visible that is we call selected. we won't select
 * anymore if all the controller is invisible that is we call unselected. we
 * will select the Top one if isMultiple is false we will unselect all the
 * controller. so that if the user click outside all the controller he will
 * unselect all controller
 * @param scenePoint
 * @param isMultiple
 */
void MainStageScene::handleSelectClick(QPointF scenePoint, bool isMultiple) {
  if (this->sceneMode == SceneMode::EDIT) {
    return;
  }
  bool shouldSelect = true;
  std::vector<AbstractController*> hitController;
  this->controllerRoot->forEachController([&](auto* controller) {
    auto localPos = controller->mapFromScene(scenePoint);
    if (controller->contains(localPos)) {
      if (controller->isControllerSelected()) {
        shouldSelect = false;
        return false;
      }
      if (controller->isControllerSelectAble()) {
        hitController.push_back(controller);
      }
    }
    return true;
  });
  if (!shouldSelect) {
    return;
  }

  // we need to get the item now still visible to emit the sections
  auto visibleItems = this->controllerRoot->getSelectedChildren();

  auto nowSelectItemId = std::vector<int>();
  for (const auto& visItem : visibleItems) {
    if (!isMultiple) {
      visItem->unSelectTheController();
    } else {
      nowSelectItemId.push_back(visItem->controllerId());
    }
  }

  if (!hitController.empty()) {
    hitController[0]->selectTheController();
    nowSelectItemId.push_back(hitController[0]->controllerId());
  }
  emit selectionChanged(nowSelectItemId);
}

void MainStageScene::selectLayers(const std::vector<int>& selectionId) {
  this->controllerRoot->forEachController([&](auto* controller) {
    if (std::find(selectionId.begin(), selectionId.end(),
                  controller->controllerId()) != selectionId.end()) {
      controller->selectTheController();
    } else {
      controller->unSelectTheController();
    }
    return true;
  });
}

void MainStageScene::setSceneUndoStack(QUndoStack* undoStack) {
  this->controllerRoot->setUndoStack(undoStack);
}

void MainStageScene::handleToolChanged(int index) {
  switch (index) {
    // 0 or 1 is the select controller change
    case 0:
      controllerRoot->setActiveSelectController(RectController);
      break;
    case 1:
      controllerRoot->setActiveSelectController(RotationController);
      break;
    case 2:
      controllerRoot->setActiveSelectController(PenController);
      break;
    default:
      break;
  }
}

void MainStageScene::drawBackground(QPainter* painter, const QRectF& rect) {
  Q_UNUSED(rect)

  painter->beginNativePainting();
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  painter->endNativePainting();
  QGraphicsScene::drawBackground(painter, rect);
}

void MainStageScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  handler->mousePressEvent(event);
}

void MainStageScene::keyPressEvent(QKeyEvent* event) {
  handler->keyPressEvent(event);
}

void MainStageScene::keyReleaseEvent(QKeyEvent* event) {
  handler->keyReleaseEvent(event);
}
}  // namespace Scene
