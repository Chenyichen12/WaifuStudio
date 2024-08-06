#include "mainglstage.h"

#include <QOpenGLFunctions>

#include "QDragMoveEvent"
views::MainGlViewPort::MainGlViewPort(QWidget* parent) : QOpenGLWidget(parent) {
  auto format = QSurfaceFormat();
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  setFormat(format);
}
void views::MainGlViewPort::initializeGL() {
  QOpenGLWidget::initializeGL();
  initializeOpenGLFunctions();
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_BLEND_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                      GL_ONE);
  emit glHasInit();
}

views::MainGlGraphicsView::MainGlGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
  this->setDragMode(DragMode::NoDrag);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}
void views::MainGlGraphicsView::keyPressEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    if (event->key() == Qt::Key_Space) {
      this->setDragMode(DragMode::ScrollHandDrag);
    }
  }
  QGraphicsView::keyPressEvent(event);
}
void views::MainGlGraphicsView::keyReleaseEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    this->setDragMode(DragMode::NoDrag);
  }
  QGraphicsView::keyReleaseEvent(event);
}
void views::MainGlGraphicsView::wheelEvent(QWheelEvent* event) {
  int angle = event->angleDelta().y();
  double factor = angle > 0 ? 1.1 : 0.9;
  scale(factor, factor);
}
