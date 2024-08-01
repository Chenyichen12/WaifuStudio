//
// Created by chen_yichen on 2024/7/30.
//
#pragma once
#include <QMainWindow>

#include "model/project.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

private:
  ProjectModel::Project *currentProject;

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  void setProject(ProjectModel::Project*);

 private:
  Ui::MainWindow *ui;
};

