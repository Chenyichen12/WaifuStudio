#pragma once
#include <QWidget>

class QPushButton;
namespace views {
class MainStageSideToolBar : public QWidget {
  Q_OBJECT

 private:
  int currentBtnIndex;
  QList<QPushButton*> toolList;

  /**
   * the side bar tool always can select one, call it to switch tool
   * @param index the tool to switch
   */
  void handleSwitchTool(int index);
  static QString generateButtonStyleSheet(const QString& iconPath);
  QPushButton* addBtn(const QString& iconPath);

 public:
  MainStageSideToolBar(QWidget* parent = nullptr);
  void setPositionFromRect(const QRect& rect);

  int getCurrentTool() const;

  void setEnableTool(int index, bool enable);
 signals:
  /**
   * signal when switch tool in the sidebar
   * @param index the tool index to switch
   */
  void switchTool(int index);
};
}  // namespace views
