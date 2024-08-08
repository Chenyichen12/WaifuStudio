//
// Created by chen_yichen on 2024/7/30.
//
#include "psdparser.h"

#include <QLabel>
#include <QHBoxLayout>
#include "filesystem"
#include "gtest/gtest.h"
TEST(psdparser, testparse) {
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "colorexample.psd";
  auto resString = QString::fromStdString(resPath.string());
  auto p = new Parser::PsdParser(resString);
  p->Parse();

  auto psTree = p->extractPsTree();
  ASSERT_EQ(psTree->itemChild(psTree->invisibleRootItem()).size(), 4);
  delete p;
}
