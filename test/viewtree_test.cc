#include <gtest/gtest.h>
#include <psdparser.h>
#include <viewtree_model.h>

#include <filesystem>

// TEST(TreeModel, TestFrom) {
//   std::string s = __FILE__;
//   std::filesystem::path pathObj(s);
//   std::filesystem::path resPath =
//       pathObj.parent_path() / "test_res" / "example.psd";
//   auto resString = QString::fromStdString(resPath.string());
//   auto p = new Parser::PsdParser(resString);
//   p->Parse();
//   auto psTree = p->extractPsTree();
//
//   auto model = View::TreeModel::fromManager(psTree);
//   auto count = model->rowCount();
//   auto p1 = model->item(0);
//   auto firstItemPtr = dynamic_cast<View::TreeItem*>(model->item(0));
//
//   ASSERT_NE(firstItemPtr, nullptr);
//
//   ASSERT_EQ(firstItemPtr->getId(),
//             psTree->getRoot()->getChildren()[0]->getData()->getId());
//
//   delete psTree;
//   delete p;
// }
