#include <QGraphicsView>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
namespace views {
class MainGlViewPort;

class MainGlGraphicsView : public QGraphicsView {
  Q_OBJECT
private:
  MainGlViewPort* glViewport;
 public:
  explicit MainGlGraphicsView(QWidget* parent = nullptr);
  void makeCurrent();
  void doneCurrent();
 signals:
 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

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
