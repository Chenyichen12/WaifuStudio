#include <QGraphicsView>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
namespace views {
class MainGlViewPort;

class MainGlGraphicsView : public QGraphicsView {
  Q_OBJECT
 private:
  MainGlViewPort* glViewport;
  QRectF sceneRubberRect;

 public:
  explicit MainGlGraphicsView(QWidget* parent = nullptr);
  void makeCurrent();
  void doneCurrent();
 signals:
  void rubberSelected(QRectF rubberRect);

 protected:
  void keyPressEvent(QKeyEvent* event) override;
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
