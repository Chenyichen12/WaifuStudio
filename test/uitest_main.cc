
#include <QApplication>
#include <QDialog>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <filesystem>

#include "mainwindow.h"
#include "model/projectservice.h"
#include "model/scene/deformer/rectdeformer.h"
#include "model/scene/mainstagescene.h"

class ProjectServiceTest : public QObject {
  WaifuL2d::ProjectService* service;

 public:
  explicit ProjectServiceTest(WaifuL2d::ProjectService* service,
                              QObject* parent = nullptr)
      : QObject(parent), service(service) {}
  void addRectDeformer() {
    auto scene = service->project->scene;
    auto rect = new WaifuL2d::RectDeformer({0, 0, 1000, 1000}, 3, 3);
    scene->addDeformer(rect);
  }
};

class MainWindowTest : public QDialog {
 private:
  MainWindow* w;
  ProjectServiceTest* serviceTest;

 public:
  explicit MainWindowTest(MainWindow* w) : QDialog(w), w(w) {
    setMinimumSize(400, 300);
    move(0, 0);
    auto basicLayout = new QVBoxLayout(this);
    serviceTest = new ProjectServiceTest(w->projectService, this);

    auto momoChangeTest = new QPushButton("change to momo", this);
    connect(momoChangeTest, &QPushButton::clicked, this,
            &MainWindowTest::testChangeMomo);
    auto addRectDeformer = new QPushButton("add rect deformer", this);
    connect(addRectDeformer, &QPushButton::clicked, this,
            [this]() { serviceTest->addRectDeformer(); });

    basicLayout->addWidget(momoChangeTest);
    basicLayout->addWidget(addRectDeformer);
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
