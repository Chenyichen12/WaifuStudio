//
// Created by chen_yichen on 2024/7/30.
//
#include "mainwindow.h"

#include "model/layer_model.h"
#include "model/mainstagescene.h"
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
  auto glViewPort = new views::MainGlViewPort();
  ui->MainStageGraphicsView->setViewport(glViewPort);
  connect(glViewPort, &views::MainGlViewPort::glHasInit, this,
          [&]() { emit this->windowInited(); });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setProject(ProjectModel::Project* p) {
  if (this->currentProject != nullptr) {
    this->currentProject->deleteLater();
  }
  this->currentProject = p;
  currentProject->setParent(this);
  this->setUpTreeModel(p->getLayerModel());
  this->setUpMainStage();
}

void MainWindow::setUpProjectFromPsd(const QString& path) {
  Parser::PsdParser* parser = new Parser::PsdParser(path);
  auto&& builder = ProjectModel::ProjectBuilder();
  parser->Parse();
  builder.setBitmapManager(parser->extractBitmapManager());
  builder.setLayerModel(new ProjectModel::LayerModel(
      parser->extractPsTree(), parser->extractControllerTree()));
  builder.projectWidth = parser->width();
  builder.projectHeight = parser->height();
  auto project = builder.build();
  this->setProject(project);
  parser->deleteLater();
}
void MainWindow::setUpMainStage() {
  ui->MainStageGraphicsView->setScene(currentProject->getScene());
  currentProject->getScene()->setUpGL();
  //  connect(glViewPort,&views::MainGlViewPort::glHasInit,this,&MainWindow::windowInited);
  //  connect(glViewPort, &views::MainGlViewPort::glHasInit,
  //          currentProject->getScene(),
  //          [&]() { currentProject->getScene()->setUpGL(); });
}
