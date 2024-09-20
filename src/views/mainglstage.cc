#include "mainglstage.h"

#include <QOpenGLFunctions>

#include "QDragMoveEvent"
#include "mainstagesidetoolbar.h"
views::MainGlViewPort::MainGlViewPort(QWidget* parent) : QOpenGLWidget(parent) {
  auto format = QSurfaceFormat();
  format.setDepthBufferSize(32);
  format.setMajorVersion(3);
  format.setMinorVersion(3);
  format.setProfile(QSurfaceFormat::CoreProfile);
#ifdef Q_OS_MAC
  QSurfaceFormat::setDefaultFormat(format);
#endif
  setFormat(format);
}
void views::MainGlViewPort::initializeGL() {
  QOpenGLWidget::initializeGL();
  initializeOpenGLFunctions();
  emit glHasInit();
}

views::MainGlGraphicsView::MainGlGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
  this->setDragMode(DragMode::NoDrag);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  this->setDragMode(DragMode::RubberBandDrag);

  connect(this, &MainGlGraphicsView::rubberBandChanged, this,
          &MainGlGraphicsView::handleRubberChanged);
  this->glViewport = new MainGlViewPort(this);
  this->setViewport(glViewport);

  this->toolBar = new MainStageSideToolBar(this);
}

void views::MainGlGraphicsView::resizeEvent(QResizeEvent* event) {
  QGraphicsView::resizeEvent(event);
  toolBar->setPositionFromRect(this->viewport()->rect());
}

void views::MainGlGraphicsView::keyPressEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    if (event->key() == Qt::Key_Space) {
      this->setDragMode(DragMode::ScrollHandDrag);
    }
  }
  QGraphicsView::keyPressEvent(event);
}

/**
 * mouse handle event
 * use to distinguish the click and drag
 * @param event
 */
void views::MainGlGraphicsView::mousePressEvent(QMouseEvent* event) {
  QGraphicsView::mousePressEvent(event);
  isMousePressed = true;
}

void views::MainGlGraphicsView::mouseMoveEvent(QMouseEvent* event) {
  QGraphicsView::mouseMoveEvent(event);
  if (isMousePressed) {
    isMousePressAndMove = true;
  }
}

void views::MainGlGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
  QGraphicsView::mouseReleaseEvent(event);
  if (!isMousePressAndMove) {
    emit mouseSelectClick(mapToScene(event->pos()),
                          event->modifiers() == Qt::ShiftModifier);
  }
  isMousePressAndMove = false;
  isMousePressed = false;
}

void views::MainGlGraphicsView::keyReleaseEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    this->setDragMode(DragMode::RubberBandDrag);
  }
  QGraphicsView::keyReleaseEvent(event);
}
void views::MainGlGraphicsView::wheelEvent(QWheelEvent* event) {
  int angle = event->angleDelta().y();
  double factor = angle > 0 ? 1.1 : 0.9;
  scale(factor, factor);
}

void views::MainGlGraphicsView::handleRubberChanged(QRect rubberBandRect,
                                                    QPointF fromScenePoint,
                                                    QPointF toScenePoint) {
  if (rubberBandRect.width() == 0 || rubberBandRect.height() == 0) {
    emit rubberSelected(this->sceneRubberRect);
  } else {
    this->sceneRubberRect = QRectF(fromScenePoint, toScenePoint);
  }
}

void views::MainGlGraphicsView::makeCurrent() { glViewport->makeCurrent(); }

void views::MainGlGraphicsView::doneCurrent() { glViewport->doneCurrent(); }

views::MainStageSideToolBar* views::MainGlGraphicsView::getToolBar() const {
  return this->toolBar;
}