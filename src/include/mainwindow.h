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

class MainWindow : public QMainWindow {
  Q_OBJECT
 private:
  void setUpMenu();
  WaifuL2d::ProjectService *projectService;
 protected slots:
  void handleProjectChanged();

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
