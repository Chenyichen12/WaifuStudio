//
// Created by chen_yichen on 2024/7/30.
//
#pragma once
#include <QMainWindow>

namespace ProjectModel {
class LayerModel;
class Project;
}  // namespace ProjectModel

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 private:
  ProjectModel::Project *currentProject = nullptr;

  void setUpTreeModel(const ProjectModel::LayerModel *m);
  void setUpMainStage();
  void setUpMenu();

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  void setProject(ProjectModel::Project *);
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
