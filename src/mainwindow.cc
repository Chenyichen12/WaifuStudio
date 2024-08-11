//
// Created by chen_yichen on 2024/7/30.
//
#include "mainwindow.h"

#include "QFileDialog"
#include "model/layer_model.h"
#include "model/project.h"
#include "model/scene/mainstagescene.h"
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
  setUpMenu();
  this->currentProject = nullptr;
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setProject(ProjectModel::Project* p) {
  if (this->currentProject != nullptr) {
    delete currentProject;
    currentProject = nullptr;
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
  auto p = builder.build();
  this->setProject(p);
  parser->deleteLater();
}

void MainWindow::handlePsdOpen() {
  auto fileName = QFileDialog::getOpenFileName(this, tr("open psd file"), "",
                                               "PsFiles (*.psd *.psb)");
  if (fileName == "") return;

  setUpProjectFromPsd(fileName);
}

void MainWindow::setUpMainStage() {
  ui->MainStageGraphicsView->setScene(currentProject->getScene());
  connect(ui->MainStageGraphicsView, &views::MainGlGraphicsView::rubberSelected,
          currentProject->getScene(),
          &Scene::MainStageScene::handleRubberSelect);
  connect(
      ui->MainStageGraphicsView, &views::MainGlGraphicsView::mouseSelectClick,
      currentProject->getScene(), &Scene::MainStageScene::handleSelectClick);
  ui->MainStageGraphicsView->makeCurrent();
  currentProject->getScene()->initGL();
  ui->MainStageGraphicsView->doneCurrent();
}

void MainWindow::setUpMenu() {
  auto openMenu = ui->menubar->addMenu(tr("Open"));
  openMenu->addAction(tr("Open from ps file"), this,
                                           &MainWindow::handlePsdOpen);
}
