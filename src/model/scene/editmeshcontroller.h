#pragma once
#include "mesh.h"
#include "scenecontroller.h"
namespace Scene {
class EditMeshController : public AbstractController {
 private:
  std::vector<MeshVertex> vertices;
  std::vector<int> incident;


  std::vector<int> selectIndex;
 public:
  EditMeshController(const std::vector<MeshVertex>& vertices,
                     const std::vector<int>& incident,
                     QGraphicsItem* parent = nullptr);
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;
  QRectF boundingRect() const override;
};
}  // namespace Scene
