//
// Created by chen_yichen on 2024/7/30.
//
#include "mainwindow.h"

#include <QItemSelectionModel>
#include <QStandardItemModel>

#include "QFileDialog"
#include "model/projectservice.h"
#include "ui/ui_mainwindow.h"
#include "views/mainstagesidetoolbar.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setUpMenu();
  this->projectService = new WaifuL2d::ProjectService(this);

  connect(projectService, &WaifuL2d::ProjectService::projectChanged, this,
          &MainWindow::handleProjectChanged);
}

void MainWindow::handleProjectChanged() {
  ui->controllerTree->setModel(projectService->getLayerModel());
  ui->controllerTree->setSelectionModel(projectService->getLayerSelectionModel());
}

MainWindow::~MainWindow() { delete ui; }


void MainWindow::setUpProjectFromPsd(const QString& path) {
  projectService->initProjectFromPsd(path);
}

void MainWindow::handlePsdOpen() {
  auto fileName = QFileDialog::getOpenFileName(this, tr("open psd file"), "",
                                               "PsFiles (*.psd *.psb)");
  if (fileName == "") return;

  setUpProjectFromPsd(fileName);
}

void MainWindow::handleUndoAction() {
}
void MainWindow::handleRedoAction() {
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
