//
// Created by chenyichen on 8/7/24.
//
#pragma once

#include <glm/glm.hpp>

#include "../layer_bitmap.h"
#include "QGraphicsItem"
#include "QOpenGLFunctions"
#include "QOpenGLTexture"
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
namespace Scene {
class MeshRenderGroup;
struct MeshVertex {
  glm::vec2 pos;
  glm::vec2 uv;
  float x() const { return pos.x; }
  float y() const { return pos.y; }
};
enum MeshVertexOffset {
  Position = offsetof(MeshVertex, pos),
  UV = offsetof(MeshVertex, uv)
};
class Mesh : public QGraphicsItem, protected QOpenGLFunctions {
  friend MeshRenderGroup;  // the group manager

 private:
  /**
   * the vertex information in project
   */
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> incident;
  QRectF boundRect;
  int layerId = 0;
  QImage image;  // the raw image in scene

 protected:
  /**
   * gl data struct it should not be changed by outside
   */
  QOpenGLVertexArrayObject* vao;
  QOpenGLBuffer* vbo;
  QOpenGLBuffer* ibo;
  QOpenGLTexture* tex;

  virtual void initializeGL(QRect relativeRect);

 public:
  /**
   * the actual render gl mesh
   * @param vertices mesh only accept the scene position which is relative to
   * project
   * @param incident gl incident
   */
  Mesh(const std::vector<MeshVertex>& vertices,
       const std::vector<unsigned int>& incident,
       MeshRenderGroup* parent = nullptr);
  ~Mesh() override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  const std::vector<MeshVertex>& getVertices() const { return vertices; }

  const std::vector<unsigned int>& getIncident() const { return incident; }

  void setVerticesAt(int index, const MeshVertex& vertex);

  /**
   * when the vertex change, should call upDate to refresh the gl buffer
   */
  virtual void upDateBuffer();
  /**
   * renew the buffer object to fit new size
   * usually used when mesh has struct changed
   */
  virtual void resetBuffer();

  void setTexture(const QImage& image);
  /**
   * get the texture image of this mesh
   * it can also be found in layerbitmap manager
   * @return 
   */
  QImage getTextureImage() const;
  /**
   * the id which is made by project. it should be called only once
   */
  void bindId(int id) { layerId = id; }

  int getLayerId() const { return layerId; }

  /**
   * set point for mesh, point from scene
   * the uv will auto calculate from previous mesh information
   * @param points points position
   * @param incident incident
   */
  void setMeshPointFromScene(const std::vector<QPointF>& points, const std::vector<unsigned int>& incident);
};

class MeshBuilder {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> verticesIndex;
  std::unique_ptr<QImage> bitmapImage;

 public:
  /**
   * the default mesh to build to show the project bitmap.
   */
  void setUpDefault(const ProjectModel::BitmapAsset* bitmap);

  /**
   * specify the vertices and index to build the mesh.
   * when you call the setUpVertices, you should also call setUpTexture and not
   * call setUpDefault
   * @param vertices vertices information
   * @param index incident
   */
  void setUpVertices(const std::vector<MeshVertex>& vertices,
                     const std::vector<unsigned int>& index);
  void setUpTexture(const QImage& image);

  /**
   * build the actual mesh
   * @return
   */
  virtual std::unique_ptr<Mesh> buildMesh();
};
}  // namespace Scene
