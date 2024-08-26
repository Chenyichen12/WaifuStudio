#pragma once
#include <QObject>

namespace Scene {
class MainStageScene;
}

namespace ProjectModel {
class LayerModel;
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
  /**
   * in some time we should disable some widget when enter the edit model
   * such as the tree view to defence to modify  of the layer
   */
  QList<QWidget*> disabledWidgets;
 public:
  EditModeController(Scene::MainStageScene* scene,
                     ProjectModel::LayerModel* layerModel,
                     views::MainGlGraphicsView* views,
                     QObject* parent = nullptr);

  void setDisabledWidget(const QList<QWidget*>& widgets);
 public slots:
  void handleEnterEditMode() const;
  void handleLeaveEditMode() const;
};
}  // namespace Controller