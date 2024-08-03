
#include "psdparser.h"
#include <filesystem>
#include <QApplication>
#include "mainwindow.h"
#include "model/layer_model.h"
#include "model/project.h"
int main(int argc, char** argv) {
  QApplication a(argc, argv);
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "example.psd";
  
  Parser::PsdParser* parser = new Parser::PsdParser(QString::fromStdString(resPath.string()));
  auto&& builder = ProjectModel::ProjectBuilder();
  parser->Parse();
  builder.setBitmapManager(parser->extractBitmapManager());
  builder.setLayerModel(new ProjectModel::LayerModel(
      parser->extractPsTree(), parser->extractControllerTree()));
  auto project = builder.build();
  auto mainwindow = new MainWindow();
  mainwindow->setProject(project);
  mainwindow->show();
  QApplication::exec();

}