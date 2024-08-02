#include "viewtree_model.h"

#include <stack>

#include "model/layer_model.h"

namespace View {
//
// QIcon convertBitmapToIcon(const ProjectModel::LayerBitmap* bitmap) {
//   auto image = QImage(bitmap->rawBytes, bitmap->width, bitmap->height,
//                       QImage::Format_RGBA8888);
//
//   image = image.scaledToHeight(20);
//   auto pixmap = QPixmap::fromImage(image);
//   return {pixmap};
// }
//
// std::unique_ptr<TreeItem> createFromNode(
//     const ProjectModel::TreeNode<ProjectModel::Layer>* node) {
//   auto item = std::make_unique<TreeItem>(node->getData()->getId());
//   item->setText(node->getData()->getName());
//
//   auto image_p =
//       std::dynamic_pointer_cast<ProjectModel::BitmapLayer>(node->getData());
//   if (image_p != nullptr) {
//     item->setIcon(convertBitmapToIcon(&image_p->getBitMap()));
//     return item;
//   }
//
//   auto group_p =
//       std::dynamic_pointer_cast<ProjectModel::PsGroupLayer>(node->getData());
//   if (group_p != nullptr) {
//     item->setIcon(QIcon(":/icon/folder.png"));
//     return item;
//   }
//
//   item->setIcon(QIcon(":/icon/question.png"));
//   return item;
// }
//
// void makeItemTree(
//     QStandardItem* parent,
//     const std::vector<ProjectModel::TreeNode<ProjectModel::Layer>*>&
//         parseNodes) {
//   for (auto&& node : parseNodes) {
//     auto item = createFromNode(node);
//     makeItemTree(item.get(), node->getChildren());
//     parent->appendRow(item.release());
//   }
// }
//
// std::unique_ptr<TreeModel> TreeModel::fromManager(
//     const ProjectModel::TreeItemModel* manager) {
//   auto ptr = std::make_unique<TreeModel>();
//
//   makeItemTree(ptr->invisibleRootItem(), manager->getRoot()->getChildren());
//
//   auto c = ptr->rowCount();
//
//   return ptr;
// }
//
// TreeModel::TreeModel(QObject* parent) : QStandardItemModel(parent) {}
// TreeItem::TreeItem(int id) { this->id = id; }
//
// int TreeItem::getId() const { return this->id; }
}  // namespace View
