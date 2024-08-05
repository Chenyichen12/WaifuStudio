
#include <QApplication>
#include <filesystem>

#include "mainwindow.h"
class InitObject : public QObject {
  Q_OBJECT
 public:
  std::string resPath;
  InitObject(MainWindow* mainWindow) {
    this->setParent(mainWindow);
    connect(mainWindow, &MainWindow::windowInited, this, [&]() {
      mainWindow->setUpProjectFromPsd(QString::fromStdString(resPath));
    });
  }
};
int main(int argc, char** argv) {
  QApplication a(argc, argv);
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "example.psd";

  auto mainwindow = new MainWindow();
  auto init = new InitObject(mainwindow);
  init->resPath = resPath;
  mainwindow->show();
  QApplication::exec();
}

#include "uitest_main.moc"