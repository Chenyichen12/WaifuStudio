//
// Created by chen_yichen on 2024/7/30.
//
#include "model/parser/psdparser.h"

#include <QLabel>
#include <QHBoxLayout>
#include "filesystem"
#include "gtest/gtest.h"
TEST(psdparser, testparse) {
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath =
      pathObj.parent_path() / "test_res" / "colorexample.psd";
  auto p = new WaifuL2d::PsdParser(resPath.wstring());
  p->parse();

  auto psTree = p->getResult();

  ASSERT_EQ(psTree->root->children.size(),4);
  delete p;
}
