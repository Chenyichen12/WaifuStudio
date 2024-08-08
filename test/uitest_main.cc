
#include <QApplication>
#include <QSurfaceFormat>
#include <QTimer>
#include <filesystem>

#include "mainwindow.h"
int main(int argc, char** argv) {
  QApplication a(argc, argv);
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "example.psd";

  auto mainwindow = new MainWindow();
  mainwindow->show();
  mainwindow->setUpProjectFromPsd(QString::fromStdString(resPath.string()));
  QApplication::exec();
  delete mainwindow;
}
