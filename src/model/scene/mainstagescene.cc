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

MainStageScene::MainStageScene(MeshRenderGroup* group,
                               RootController* controllerGroup, QObject* parent)
    : QGraphicsScene(parent) {
  int width = group->getRenderWidth();
  int height = group->getRenderHeight();
  auto backRect = QRectF(0, 0, width, height);
  auto bound = backRect.marginsAdded(
      QMarginsF(width / 2, height / 2, width / 2, height / 2));

  boundRect = new QGraphicsRectItem(bound);
  this->addItem(boundRect);
  renderGroup = group;
  this->controllerRoot = controllerGroup;
  this->addItem(renderGroup);
  this->addItem(controllerGroup);

  this->handler = new MainStageEventHandler(this);
}

MainStageScene::~MainStageScene() { delete handler; }

void MainStageScene::initGL() {
  initializeOpenGLFunctions();
  renderGroup->initializeGL();
}

void MainStageScene::handleRubberSelect(QRectF sceneRect) {
  qDebug() << sceneRect;
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
