//
// Created by chen_yichen on 2024/7/30.
//
#include <gtest/gtest.h>
#include <tree_manager.h>

using namespace ProjectModel;
class TextLayer : public Layer {
 public:
  explicit TextLayer(const QString &name, bool isVisible = true)
      : Layer(name, isVisible) {}

 public:
  bool isContainer() override { return true; }
};

TEST(TreeManager, TestStruct) {
  auto m = TreeManager();
  auto n1 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t1")));
  auto n2 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t2")));
  auto n3 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t3")));
  auto n4 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t4")));
  auto n5 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t5")));
  auto n6 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t6")));
  auto n7 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t7")));

  n1->getChildren().push_back(n2);
  n1->getChildren().push_back(n3);
  n2->setParent(n1);
  n3->setParent(n1);

  n2->getChildren().push_back(n4);
  n4->setParent(n2);

  n4->getChildren().push_back(n5);
  n5->setParent(n4);

  n3->getChildren().push_back(n6);
  n3->getChildren().push_back(n7);

  n6->setParent(n3);
  n7->setParent(n3);

  m.getRoot()->getChildren().push_back(n1);
  n1->setParent(m.getRoot());

  auto map =
      std::array<std::string, 7>{"t1", "t2", "t4", "t5", "t3", "t6", "t7"};

  auto parentMap =
      std::array<TreeNode<Layer> *, 7>{m.getRoot(), n1, n2, n4, n1, n3, n3};
  int count = 0;
  auto asset = [&](const TreeNode<Layer> *p) {
    ASSERT_EQ(parentMap[count], p->getParent());
    ASSERT_EQ(map[count], p->getData()->getName().toStdString());
  };
  m.forEach([&](const TreeNode<Layer> *f) {
    asset(f);
    count++;
    return true;
  });
}

TEST(TreeManager, TestFind) {
  auto m = TreeManager();
  auto n1 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t1")));
  auto n2 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t2")));
  auto n3 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t3")));
  auto n4 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t4")));
  auto n5 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t5")));
  auto n6 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t6")));
  auto n7 = new TreeNode<Layer>(std::shared_ptr<Layer>(new TextLayer("t7")));

  n1->getChildren().push_back(n2);
  n1->getChildren().push_back(n3);
  n2->setParent(n1);
  n3->setParent(n1);

  n2->getChildren().push_back(n4);
  n4->setParent(n2);

  n4->getChildren().push_back(n5);
  n5->setParent(n4);

  n3->getChildren().push_back(n6);
  n3->getChildren().push_back(n7);

  n6->setParent(n3);
  n7->setParent(n3);

  m.getRoot()->getChildren().push_back(n1);
  n1->setParent(m.getRoot());

  auto &&node = m.findNode(n5->getData()->getId());
  ASSERT_EQ(node, n5);
}
