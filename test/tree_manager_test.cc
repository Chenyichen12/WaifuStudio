//
// Created by chen_yichen on 2024/7/30.
//
#include <gtest/gtest.h>
#include <tree_manager.h>
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



  m.forEach([&](const TreeNode<Layer> *f) {
    std::cout << f->getData()->getName().toStdString();
    return true;
  });

  //  auto l1 = TextLayer("t1");
  //  auto l2 = TextLayer("t2");
  //  auto l3 = TextLayer("t3");
  //  auto l4 = TextLayer("t4");
  //  auto l5 = TextLayer("t5");
  //  auto l6 = TextLayer("t6");
  //  auto l7 = TextLayer("t7");
}
