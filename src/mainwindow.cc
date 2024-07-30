//
// Created by chen_yichen on 2024/7/30.
//
#include "include/mainwindow.h"
#include "ui/ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }
