#pragma once
#include <QWidget>
#include "model/controller/scenecontroller.h"
class QPushButton;

namespace views {
class MainStageTopBar : public QWidget {
  Q_OBJECT

private:
  QPushButton* editModeBtn;
  // init when service init. it shouldn't be nullptr
  WaifuL2d::SceneController* controller;

public:
  MainStageTopBar(QWidget* parent = nullptr);
  /**
   * reset the first state
   */
  void reset();

  /**
   * set the check state of the edit button
   * will block the signals of edit mode
   * @param checked
   */
  // void setEditBtnChecked(bool checked);

  /**
   * call it when service init set controller to scene controller
   * @param controller 
   */
  void setController(WaifuL2d::SceneController* controller);

public slots:
  void handleControllerStateChanged(WaifuL2d::SceneControllerState state);
signals:
  void enterEditMode();
  void leaveEditMode();
};
} // namespace views