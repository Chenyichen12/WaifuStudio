#pragma once

#include <QGraphicsView>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
namespace views {
class MainGlViewPort;
class MainStageSideToolBar;
class MainGlGraphicsView : public QGraphicsView {
  Q_OBJECT
 private:
  MainGlViewPort* glViewport;
  QRectF sceneRubberRect;
  MainStageSideToolBar* toolBar;

  bool isMousePressed = false;
  bool isMousePressAndMove = false;

 public:
  explicit MainGlGraphicsView(QWidget* parent = nullptr);
  void makeCurrent();
  void doneCurrent();

  /**
   * @return sidebar instance of the MainStage
   */
  MainStageSideToolBar* getToolBar() const;
 signals:
  void rubberSelected(QRectF rubberRect);
  void mouseSelectClick(QPointF scenePoint, bool isMultiple = false);

 protected:
  void resizeEvent(QResizeEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
 protected slots:
  void handleRubberChanged(QRect rubberBandRect, QPointF fromScenePoint,
                           QPointF toScenePoint);
};

class MainGlViewPort : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  explicit MainGlViewPort(QWidget* parent = nullptr);
 signals:
  void glHasInit();

 protected:
  void initializeGL() override;
};
}  // namespace views
