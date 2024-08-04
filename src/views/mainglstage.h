#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QGraphicsView>
namespace views {

class MainGlGraphicsView: public QGraphicsView{
 public:
  explicit MainGlGraphicsView(QWidget* parent = nullptr);

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
};

class MainGlViewPort : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  explicit MainGlViewPort(QWidget* parent = nullptr);
};
}  // namespace views
