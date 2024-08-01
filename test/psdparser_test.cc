//
// Created by chen_yichen on 2024/7/30.
//
#include "psdparser.h"

#include <QApplication>
#include <QLabel>
#include <QHBoxLayout>
#include "filesystem"
#include "gtest/gtest.h"
TEST(psdparser, testparse) {
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "example.psd";
  auto resString = QString::fromStdString(resPath.string());
  auto p = new Parser::PsdParser(resString);
  p->Parse();
  delete p;
}

/* test parse image */
// TEST(psdparser, testimage) {
//   std::string s = __FILE__;
//   std::filesystem::path pathObj(s);
//   std::filesystem::path resPath =
//       pathObj.parent_path() / "test_res" / "example.psd";
//   auto resString = QString::fromStdString(resPath.string());
//   auto p = new Parser::PsdParser(resString);
//   p->Parse();
//   auto m = p->getControllerTree();
//
//   auto w = new QWidget();
//   auto layout = new QHBoxLayout();
//   w->setLayout(layout);
//   for (auto child : m->getRoot()->getChildren()) {
//     auto label = new QLabel();
//     auto cast = child->getData();
//     auto castPtr = std::dynamic_pointer_cast<ProjectModel::BitmapLayer>(cast);
//     auto&& bitmap = castPtr->getBitMap();
//     auto image = QImage(bitmap.rawBytes, bitmap.width, bitmap.height,
//                         QImage::Format_RGBA8888);
//     label->setPixmap(QPixmap::fromImage(image));
//     label->setParent(w);
//     layout->addWidget(label);
//   }
//   w->show();
//   QApplication::exec();
// }
