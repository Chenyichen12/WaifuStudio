//
// Created by chen_yichen on 2024/7/30.
//
#include "include/mainwindow.h"
#include "model/tree_manager.h"
#include "model/layer_model.h"
#include "ui/ui_mainwindow.h"
#include "model/project.h"
void MainWindow::setUpTreeModel(const ProjectModel::LayerModel* m) const {
  ui->psTree->setModel(m->getPsdTreeManager());
  ui->controllerTree->setModel(m->getControllerTreeManger());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setProject(const ProjectModel::Project*p) const {
  this->setUpTreeModel(p->getLayerModel());
}