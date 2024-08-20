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
#include "views/mainstagesidetoolbar.h"
void MainWindow::setUpTreeModel(const ProjectModel::LayerModel* m) {
  ui->psTree->setModel(m->getPsdTreeManager());
  ui->controllerTree->setModel(m->getControllerTreeManger());
  ui->psTree->setSelectionModel(m->getPsdTreeSelectionModel());
  ui->controllerTree->setSelectionModel(m->getControllerTreeSelectionModel());

  connect(ui->psTree, &views::LayerTreeView::shouldSetVisible, m,
          &ProjectModel::LayerModel::handleItemSetVisible);
  connect(ui->controllerTree, &views::LayerTreeView::shouldSetVisible, m,
          &ProjectModel::LayerModel::handleItemSetVisible);
  connect(ui->psTree, &views::LayerTreeView::itemVisibleEnd, m,
          &ProjectModel::LayerModel::handleVisibleSelectEnd);
  connect(ui->controllerTree, &views::LayerTreeView::itemVisibleEnd, m,
          &ProjectModel::LayerModel::handleVisibleSelectEnd);
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

// ReSharper disable once CppMemberFunctionMayBeConst
void MainWindow::handleUndoAction() {
  if (this->currentProject != nullptr) {
    this->currentProject->undo();
  }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainWindow::handleRedoAction() {
  if (this->currentProject != nullptr) {
    this->currentProject->redo();
  }
}

void MainWindow::setUpMainStage() {
  ui->MainStageGraphicsView->setScene(currentProject->getScene());
  auto toolBar = ui->MainStageGraphicsView->getToolBar();
  connect(toolBar, &views::MainStageSideToolBar::switchTool,
          currentProject->getScene(),
          &Scene::MainStageScene::handleToolChanged);
  connect(ui->MainStageGraphicsView, &views::MainGlGraphicsView::rubberSelected,
          currentProject->getScene(),
          &Scene::MainStageScene::handleRubberSelect);
  connect(
      ui->MainStageGraphicsView, &views::MainGlGraphicsView::mouseSelectClick,
      currentProject->getScene(), &Scene::MainStageScene::handleSelectClick);

  // connect the topbar edit mode handler
  connect(ui->MainStageTopBar, &views::MainStageTopBar::enterEditMode,
          currentProject->getScene(), [this]() {
            currentProject->getScene()->setSceneMode(
                Scene::MainStageScene::SceneMode::EDIT);
            // if it get into the edit mode the pen tool can be enabled
            ui->MainStageGraphicsView->getToolBar()->setEnableTool(2, true);
          });
  connect(ui->MainStageTopBar, &views::MainStageTopBar::leaveEditMode,
          currentProject->getScene(), [this]() {
            currentProject->getScene()->setSceneMode(
                Scene::MainStageScene::SceneMode::NORMAL);

            // disable the pen tool when enter into normal mode
            ui->MainStageGraphicsView->getToolBar()->setEnableTool(2, false);
          });

  ui->MainStageGraphicsView->makeCurrent();
  currentProject->getScene()->initGL();
  ui->MainStageGraphicsView->doneCurrent();
  currentProject->getScene()->handleToolChanged(0);
}

void MainWindow::setUpMenu() {
  auto openMenu = ui->menubar->addMenu(tr("Open"));
  openMenu->addAction(tr("Open from ps file"), this,
                      &MainWindow::handlePsdOpen);

  auto editMenu = ui->menubar->addMenu(tr("Edit"));
  editMenu->addAction(tr("Undo"), QKeySequence(QKeySequence::Undo), this,
                      &MainWindow::handleUndoAction);

  editMenu->addAction(tr("Redo"), QKeySequence::Redo, this,
                      &MainWindow::handleRedoAction);
}
