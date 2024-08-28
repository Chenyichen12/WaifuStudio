#include "gabrenderwidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
namespace Controller {

class RenderMesh : public Scene::Mesh {
 private:
  QOpenGLShaderProgram* program = nullptr;
  QRect renderRect;
 public:
  RenderMesh(const std::vector<Scene::MeshVertex>& vertices,
             const std::vector<unsigned int>& incident)
      : Mesh(vertices, incident) {}

  void setProgram(QOpenGLShaderProgram* program) { this->program = program; }

public:
 void initializeGL(QRect relativeRect) override {
   Mesh::initializeGL(relativeRect);
   this->renderRect = relativeRect;
 }
  void paintGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    program->bind();
    glViewport(0,0,renderRect.width(),renderRect.height());
    vao->bind();
    tex->bind(0);
    program->setUniformValue("ourTexture", 0);

    glDrawElements(GL_TRIANGLES, this->getIncident().size(), GL_UNSIGNED_INT, nullptr);
    vao->release();
  }
  void setRenderRect(const QRect& renderRect) {
   this->renderRect = renderRect;
 }
};

GabRenderWidget::GabRenderWidget(const std::vector<Scene::MeshVertex>& vertices,
                                 const std::vector<unsigned int>& incident,
                                 const QImage& rawImage, QWidget* parent)
    : QOpenGLWidget(parent) {
  this->mesh = new RenderMesh(vertices, incident);
  this->mesh->setTexture(rawImage);
  auto format = QSurfaceFormat();
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  setFormat(format);
}

GabRenderWidget::~GabRenderWidget() { delete this->mesh; }

void GabRenderWidget::initializeGL() {
  QOpenGLWidget::initializeGL();
  auto program = new QOpenGLShaderProgram(this);
  program->create();
  program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                   ":/shader/bitmapshow.vert");
  program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                   ":/shader/bitmapshow.frag");
  program->link();
  this->mesh->initializeGL(this->rect());
  this->mesh->setProgram(program);
}

void GabRenderWidget::paintGL() {
  QOpenGLWidget::paintGL();
  this->mesh->paintGL();
}

void GabRenderWidget::resizeGL(int w, int h) {
  QOpenGLWidget::resizeGL(w, h);
  this->mesh->setRenderRect(this->rect());
  this->mesh->paintGL();
}
}  // namespace Conrtoller
