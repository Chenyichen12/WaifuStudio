#include "layertreeview.h"

#include <QMouseEvent>
#include <QPainter>

#include "../model/layer.h"
void views::ItemStyleDelegate::paint(QPainter* painter,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
  QStyledItemDelegate::paint(painter, option, index);
  auto is_visible = index.data(ProjectModel::VisibleRole).toBool();

  auto visible_img = is_visible ? visibleImg : invisibleImg;
  painter->drawImage(getVisibleBtnRect(option.rect), visible_img);
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
  visButtonLength = 20;
}

bool views::ItemStyleDelegate::editorEvent(QEvent* event,
                                           QAbstractItemModel* model,
                                           const QStyleOptionViewItem& option,
                                           const QModelIndex& index) {
  auto visBtnPos = getVisibleBtnRect(option.rect);
  switch (event->type()) {
    case QEvent::MouseButtonPress: {
      auto mouseEvent = static_cast<QMouseEvent*>(event);
      if (visBtnPos.contains(mouseEvent->pos())) {
        emit itemVisiblePressed(index);
      }
      break;
    }
    case QEvent::MouseMove: {
      auto mouseEvent = static_cast<QMouseEvent*>(event);
      if (visBtnPos.contains(mouseEvent->pos())) {
        emit itemVisibleMoved(index);
      }
      break;
    }
    default:
      break;
  }
  return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect views::ItemStyleDelegate::getVisibleBtnRect(const QRect& relative) const {
  auto startLeft = relative.right() - 30;
  auto startTop = relative.height() / 2 - 10 + relative.top();

  return {startLeft, startTop, visButtonLength, visButtonLength};
}

views::LayerTreeView::LayerTreeView(QWidget* parent) : QTreeView(parent) {
  this->setDragEnabled(true);
  this->setAcceptDrops(true);
  this->setDropIndicatorShown(true);
  this->setDefaultDropAction(Qt::MoveAction);

  auto styleDelegate = new ItemStyleDelegate(this);
  this->setItemDelegate(styleDelegate);

  connect(styleDelegate, &ItemStyleDelegate::itemVisiblePressed, this,
          &LayerTreeView::hanleItemVisiblePressed);
  connect(styleDelegate, &ItemStyleDelegate::itemVisibleMoved, this,
          &LayerTreeView::handleItemVisibleMoved);

}

void views::LayerTreeView::mouseReleaseEvent(QMouseEvent* event) {
  QTreeView::mouseReleaseEvent(event);
  inVisibleDrag = false;
  cacheVisibleChangedIndex.clear();
}

void views::LayerTreeView::startDrag(Qt::DropActions supportedActions) {
  if (inVisibleDrag) {
    return;
  }
  QTreeView::startDrag(supportedActions);
}

void views::LayerTreeView::hanleItemVisiblePressed(const QModelIndex& index) {
  this->inVisibleDrag = true;
  cacheVisibleChangedIndex.push_back(index);
  this->model()->setData(index, !index.data(ProjectModel::VisibleRole).toBool(),
                         ProjectModel::VisibleRole);
}

void views::LayerTreeView::handleItemVisibleMoved(const QModelIndex& index) {
  auto findItem = std::find(cacheVisibleChangedIndex.begin(),
                            cacheVisibleChangedIndex.end(), index);
  if (findItem != cacheVisibleChangedIndex.end()) {
    return;
  }

  this->model()->setData(index, !index.data(ProjectModel::VisibleRole).toBool(),
                         ProjectModel::VisibleRole);
  cacheVisibleChangedIndex.push_back(index);
}
