
#include <QApplication>
#include <QDialog>
#include <QPushButton>
#include <QTimer>
#include <filesystem>

#include "mainwindow.h"
#include <QVBoxLayout>

class MainWindowTest : public QDialog {
 private:
  MainWindow* w;

 public:
  explicit MainWindowTest(MainWindow* w) : QDialog(w), w(w) {
    setMinimumSize(400, 300);
    move(0, 0);
    auto basicLayout = new QVBoxLayout(this);

    auto momoChangeTest = new QPushButton("change to momo", this);
    connect(momoChangeTest, &QPushButton::clicked, this,
            &MainWindowTest::testChangeMomo);

    basicLayout->addWidget(momoChangeTest);
    setLayout(basicLayout);
    this->show();
  }
 public slots:
  void testChangeMomo() const {
    std::string s = __FILE__;
    std::filesystem::path pathObj(s);
    std::filesystem::path resPath =
        pathObj.parent_path() / "test_res" / "example.psd";
    w->setUpProjectFromPsd(QString::fromStdString(resPath.string()));
  }
};
int main(int argc, char** argv) {
  QApplication a(argc, argv);
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "colorexample.psd";

  auto mainwindow = new MainWindow();
  auto test = new MainWindowTest(mainwindow);

  mainwindow->show();
  mainwindow->setUpProjectFromPsd(QString::fromStdString(resPath.string()));
  QApplication::exec();
  delete mainwindow;
}
