//
// Created by chen_yichen on 2024/7/30.
//
#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
namespace WaifuL2d {
class ProjectService;
}
#ifdef QT_DEBUG
class MainWindowTest;
#endif

class MainWindow : public QMainWindow {
  Q_OBJECT
#ifdef QT_DEBUG
  friend MainWindowTest;
#endif

 private:
  void setUpMenu();
  WaifuL2d::ProjectService *projectService;
 protected slots:
  void handleProjectChanged();

 protected:
  void closeEvent(QCloseEvent *event) override;
 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  void setUpProjectFromPsd(const QString &path);

 public slots:
  void handlePsdOpen();
  void handleUndoAction();
  void handleRedoAction();
 signals:
  void windowInited();

 private:
  Ui::MainWindow *ui;
};
