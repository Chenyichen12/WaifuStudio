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
class MainStageTopBar;
}

class QWidget;

namespace Controller {
class EditModeController : public QObject {
 private:
  Scene::MainStageScene* scene;
  ProjectModel::LayerModel* layerModel;

  // should check the nullptr of these optional object
  views::MainGlGraphicsView* view = nullptr;
  views::MainStageTopBar* topBar = nullptr;
  Scene::EditMeshController* currentEditMeshController = nullptr;
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

  void handleFailLeaveEditMode() const;

 public:
  EditModeController(Scene::MainStageScene* scene,
                     ProjectModel::LayerModel* layerModel,
                     QObject* parent = nullptr);
  ~EditModeController() override;

  /**
   * set the disabled widget when enter edit
   * @param widgets
   */
  void setDisabledWidget(const QList<QWidget*>& widgets);

  /**
   * set the view
   * will auto add view toolbar in edit mode and hide when leave
   * @param view 
   */
  void setView(views::MainGlGraphicsView* view);

  /**
   * set the toolBar
   * will auto set the edit mode button checked state when enter or leave or error
   * and auto connect the enter and leave edit mode signal
   * @param topBar 
   */
  void setTopBar(views::MainStageTopBar* topBar);

  bool isEditMode() const { return currentEditMeshController != nullptr; }
 public slots:
  void handleEnterEditMode();
  void handleLeaveEditMode();
};
}  // namespace Controller