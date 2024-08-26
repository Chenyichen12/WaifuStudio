#pragma once
#include <QObject>

namespace Scene {
class MainStageScene;
class EditMeshController;
}

namespace ProjectModel {
class LayerModel;
class BitmapLayer;
}

namespace views {
class MainGlGraphicsView;
}

class QWidget;

namespace Controller {
class EditModeController : public QObject {
 private:
  Scene::MainStageScene* scene;
  ProjectModel::LayerModel* layerModel;
  views::MainGlGraphicsView* view;
  Scene::EditMeshController* currentEditController;
  /**
   * in some time we should disable some widget when enter the edit model
   * such as the tree view to defence to modify  of the layer
   */
  QList<QWidget*> disabledWidgets;
  /**
   * to get the first select layer
   * the layer is bitmap layer which can be edited
   * if no select, it will return the first bitmap layer's id of the controller
   * model
   * @return
   */
  ProjectModel::BitmapLayer* getFirstSelectLayer() const;
 public:
  EditModeController(Scene::MainStageScene* scene,
                     ProjectModel::LayerModel* layerModel,
                     views::MainGlGraphicsView* views,
                     QObject* parent = nullptr);

  /**
   * set the disabled widget when enter edit
   * @param widgets
   */
  void setDisabledWidget(const QList<QWidget*>& widgets);
 public slots:
  void handleEnterEditMode();
  void handleLeaveEditMode();
};
}  // namespace Controller