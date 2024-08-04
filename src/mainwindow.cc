//
// Created by chen_yichen on 2024/7/30.
//
#include "mainwindow.h"

#include "model/layer_model.h"
#include "model/project.h"
#include "psdparser.h"
#include "ui/ui_mainwindow.h"
void MainWindow::setUpTreeModel(const ProjectModel::LayerModel* m) {
  ui->psTree->setModel(m->getPsdTreeManager());
  ui->controllerTree->setModel(m->getControllerTreeManger());
  connect(ui->psTree, &views::LayerTreeView::shouldSetVisible, m,
          &ProjectModel::LayerModel::setItemVisible);
  connect(ui->controllerTree, &views::LayerTreeView::shouldSetVisible, m,
          &ProjectModel::LayerModel::setItemVisible);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setProject(const ProjectModel::Project* p) {
  this->setUpTreeModel(p->getLayerModel());
}

void MainWindow::setUpProjectFromPsd(const QString& path) {
  Parser::PsdParser* parser = new Parser::PsdParser(path);
  auto&& builder = ProjectModel::ProjectBuilder();
  parser->Parse();
  builder.setBitmapManager(parser->extractBitmapManager());
  builder.setLayerModel(new ProjectModel::LayerModel(
      parser->extractPsTree(), parser->extractControllerTree()));
  auto project = builder.build();
  this->setProject(project);
  parser->deleteLater();
}
