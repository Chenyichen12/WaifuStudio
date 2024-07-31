//
// Created by chen_yichen on 2024/7/30.
//
#include "gtest/gtest.h"
#include "iostream"
#include "filesystem"
#include "psdparser.h"
TEST(psdparser,testparse){
  std::string s = __FILE__;
  std::filesystem::path pathObj(s);
  std::filesystem::path resPath = pathObj.parent_path() / "test_res" / "example.psd";
  auto resString = QString::fromStdString(resPath.string());
  auto p = new Parser::PsdParser(resString);
  p->Parse();
  delete p;
}
