
#include <QApplication>
#include <filesystem>

#include "mainwindow.h"
int main(int argc, char** argv) {
  QApplication a(argc, argv);
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "example.psd";

  auto mainwindow = new MainWindow();
  QObject::connect(mainwindow, &MainWindow::windowInited, mainwindow, [&]() {
    mainwindow->setUpProjectFromPsd(QString::fromStdString(resPath.string()));
  });
  mainwindow->show();
  QApplication::exec();
  delete mainwindow;
}
