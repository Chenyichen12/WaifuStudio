//
// Created by chen_yichen on 2024/7/30.
//
#include "mainwindow.h"

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QStandardItemModel>

#include "QFileDialog"
#include "model/projectservice.h"
#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setUpMenu();
  this->projectService = new WaifuL2d::ProjectService(this);

  connect(projectService, &WaifuL2d::ProjectService::projectChanged, this,
          &MainWindow::handleProjectChanged);

  connect(ui->controllerTree, &views::LayerTreeView::shouldSetVisible,
          projectService, &WaifuL2d::ProjectService::setLayerVisible);
  connect(ui->controllerTree, &views::LayerTreeView::shouldSetLock,
          projectService, &WaifuL2d::ProjectService::setLayerLock);
  // set controller to topbar
  auto sceneController = projectService->getSceneController();
  ui->MainStageTopBar->setController(sceneController);
}

void MainWindow::handleProjectChanged() {
  ui->controllerTree->setModel(projectService->getLayerModel());
  ui->controllerTree->setSelectionModel(
      projectService->getLayerSelectionModel());

  ui->MainStageGraphicsView->setScene(projectService->getScene());
  ui->MainStageGraphicsView->makeCurrent();
  projectService->initGL();
  ui->MainStageGraphicsView->doneCurrent();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setUpProjectFromPsd(const QString& path) {
  auto psd = projectService->initProjectFromPsd(path);
  if (psd != 0) {
    // handle error
    QMessageBox::warning(this, tr("Error"), tr("Failed to open psd file"));
    return;
  }
}

void MainWindow::handlePsdOpen() {
  auto fileName = QFileDialog::getOpenFileName(this, tr("open psd file"), "",
                                               "PsFiles (*.psd *.psb)");
  if (fileName == "") {
    return;
  }

  setUpProjectFromPsd(fileName);
}

void MainWindow::handleUndoAction() { projectService->undo(); }
void MainWindow::handleRedoAction() { projectService->redo(); }

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