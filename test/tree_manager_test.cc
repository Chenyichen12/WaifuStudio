//
// Created by chen_yichen on 2024/7/30.
//
#include <gtest/gtest.h>
#include <model/tree_manager.h>
#include "model/layer.h"
#include "model/id_allocation.h"

using namespace ProjectModel;
class TextLayer : public Layer {
 public:
  explicit TextLayer(const QString &name, bool isVisible = true)
      : Layer(IdAllocation::getInstance().allocate()) {
    this->setText(name);
    this->setVisible(isVisible);
  }

  int type() const override { return UserType + 100; }
};

TEST(TreeManager, TestStruct) {
  auto m = TreeItemModel();

  auto n1 = new TextLayer("t1");
  auto n2 = new TextLayer("t2");
  auto n3 = new TextLayer("t3");
  auto n4 = new TextLayer("t4");
  auto n5 = new TextLayer("t5");
  auto n6 = new TextLayer("t6");
  auto n7 = new TextLayer("t7");
  n1->appendRow(n2);
  n1->appendRow(n3);

  n2->appendRow(n4);

  n4->appendRow(n5);

  n3->appendRow(n6);
  n3->appendRow(n7);

  m.appendRow(n1);

  auto map =
      std::array<std::string, 7>{"t1", "t2", "t4", "t5", "t3", "t6", "t7"};

  auto parentMap =
      std::array<QStandardItem *, 7>{nullptr, n1, n2, n4, n1, n3, n3};
  int count = 0;
  auto asset = [&](const QStandardItem *p) {
    ASSERT_EQ(parentMap[count], p->parent());
    ASSERT_EQ(map[count], p->text().toStdString());
  };
  m.forEach([&](const QStandardItem *f) {
    asset(f);
    count++;
    return true;
  });
}

TEST(TreeManager, TestFind) {
  auto m = TreeItemModel();
  auto n1 = new TextLayer("t1");
  auto n2 = new TextLayer("t2");
  auto n3 = new TextLayer("t3");
  auto n4 = new TextLayer("t4");
  auto n5 = new TextLayer("t5");
  auto n6 = new TextLayer("t6");
  auto n7 = new TextLayer("t7");

  n1->appendRow(n2);
  n1->appendRow(n3);

  n2->appendRow(n4);

  n4->appendRow(n5);

  n3->appendRow(n6);
  n3->appendRow(n7);

  m.appendRow(n1);
  auto map =
      std::array<std::string, 7>{"t1", "t2", "t4", "t5", "t3", "t6", "t7"};

  auto parentMap =
      std::array<QStandardItem *, 7>{nullptr, n1, n2, n4, n1, n3, n3};

  auto &&node = m.findNode(n5->getId());
  ASSERT_EQ(node, n5);
}
