#pragma once
#include <QWidget>
class QPushButton;

namespace views {
class MainStageTopBar : public QWidget {
  Q_OBJECT
 private:
  QPushButton* editModeBtn;

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
  void setEditBtnChecked(bool checked);

 signals:
  void enterEditMode();
  void leaveEditMode();
};
}  // namespace views
