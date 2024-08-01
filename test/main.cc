//
// Created by chen_yichen on 2024/7/30.
//
#include "gtest/gtest.h"
#include <QApplication>
int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
}
