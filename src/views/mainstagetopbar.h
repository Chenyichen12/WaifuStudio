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
 signals:
  void enterEditMode();
  void leaveEditMode();
};
}  // namespace views
