#include "views/layertreeview.h"

#include <QPainter>

#include "model/layer.h"
void views::ItemStyleDelegate::paint(QPainter* painter,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
  QStyledItemDelegate::paint(painter, option, index);
  auto is_visible = index.data(ProjectModel::VisibleRole).toBool();

  auto visible_img = is_visible ? visibleImg : invisibleImg;
  auto startLeft = option.rect.right() - 30;
  auto startTop = option.rect.height() / 2 - 10 + option.rect.top();

  painter->drawImage(QRectF(QPointF(startLeft, startTop), QSizeF(20, 20)),
                     visible_img);
}

QSize views::ItemStyleDelegate::sizeHint(const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
  auto size = QStyledItemDelegate::sizeHint(option, index);
  return {size.width(), 40};
}

views::ItemStyleDelegate::ItemStyleDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
  visibleImg = QImage(":/icon/eye-enabled.png");
  visibleImg = visibleImg.scaledToHeight(20, Qt::SmoothTransformation);

  invisibleImg = QImage(":/icon/eye-disabled.png");

  invisibleImg = invisibleImg.scaledToHeight(20, Qt::SmoothTransformation);
}

views::LayerTreeView::LayerTreeView(QWidget* parent) : QTreeView(parent) {
  this->setDragEnabled(true);
  this->setAcceptDrops(true);
  this->setDropIndicatorShown(true);
  this->setDefaultDropAction(Qt::MoveAction);
  auto styleDelegate = new ItemStyleDelegate(this);
  this->setItemDelegate(styleDelegate);
}
