#pragma once
#include <QOpenglWidget>
#include "../scene/mesh.h"
class QOpenGLShaderProgram;
namespace Controller {
class RenderMesh;
class GabRenderWidget : public QOpenGLWidget {
  Q_OBJECT
private:
  RenderMesh* mesh;
 public:
  GabRenderWidget(const std::vector<Scene::MeshVertex>& vertices,
                  const std::vector<unsigned int>& incident,
                  const QImage& rawImage,
                  QWidget* parent = nullptr);
  ~GabRenderWidget() override;
 protected:
  
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;
};
}  // namespace Conrtoller
