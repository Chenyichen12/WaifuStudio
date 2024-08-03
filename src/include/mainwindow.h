//
// Created by chen_yichen on 2024/7/30.
//
#pragma once
#include <QMainWindow>

namespace ProjectModel {
class LayerModel;
class Project;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 private:
  ProjectModel::Project *currentProject;

  void setUpTreeModel(const ProjectModel::LayerModel* m);
 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  void setProject(const ProjectModel::Project *);
  void setUpProjectFromPsd(const QString &path);

 private:
  Ui::MainWindow *ui;
};
