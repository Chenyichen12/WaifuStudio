#pragma once
#include <QWidget>


class QPushButton;
namespace views {
class MainStageSideToolBar : public QWidget {
  Q_OBJECT

 private:
  int currentBtnIndex;
  QList<QPushButton*> toolList;

  void handleBtnPressed(int index);
 public:
  MainStageSideToolBar(QWidget* parent = nullptr);
  void setPositionFromRect(const QRect& rect);

  int getCurrentTool() const;
 signals:
  void switchTool(int index);
};
}
