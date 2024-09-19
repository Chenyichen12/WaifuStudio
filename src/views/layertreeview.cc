#include "layertreeview.h"

#include <QMouseEvent>
#include <QPainter>

#include "model/tree/layer.h"
#include <QApplication>
void views::ItemStyleDelegate::paint(QPainter* painter,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
  QStyledItemDelegate::paint(painter, option, index);
  auto is_visible =
      index.data(WaifuL2d::Layer::DataRole::LayerVisible).toBool();
  auto visibleRect = getVisibleBtnRect(option.rect);
  painter->drawRect(visibleRect);
  if (is_visible) {
    painter->drawImage(visibleRect, visibleImg);
  }

  auto is_locked = index.data(WaifuL2d::Layer::DataRole::LayerLock).toBool();
  auto lockedRect = visibleRect.translated(-visButtonLength - 5, 0);
  painter->drawRect(lockedRect);
  if (is_locked) {
    painter->drawImage(lockedRect.marginsRemoved({2, 2, 2, 2}), lockImg);
  }
}

QSize views::ItemStyleDelegate::sizeHint(const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
  auto size = QStyledItemDelegate::sizeHint(option, index);
  return {size.width(), 40};
}

bool views::ItemStyleDelegate::isLightColor(const QColor& color) {
  int r = color.red();
  int g = color.green();
  int b = color.blue();
  double brightness = 0.299 * r + 0.587 * g + 0.114 * b;
  return brightness > 128;
}

views::ItemStyleDelegate::ItemStyleDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
  visibleImg = QImage(":/icon/eye-enabled.png");
  visibleImg = visibleImg.scaledToHeight(20, Qt::SmoothTransformation);

  invisibleImg = QImage(":/icon/eye-disabled.png");

  invisibleImg = invisibleImg.scaledToHeight(20, Qt::SmoothTransformation);

  lockImg =
      QImage(":/icon/lock.png").scaledToHeight(20, Qt::SmoothTransformation);

  // light resolve
  QPalette palette = qApp->palette();
  bool isLight = isLightColor(palette.color(QPalette::Base));
  if (!isLight) {
    visibleImg.invertPixels();
    invisibleImg.invertPixels();
    lockImg.invertPixels();
  }

  visButtonLength = 20;
}

bool views::ItemStyleDelegate::editorEvent(QEvent* event,
                                           QAbstractItemModel* model,
                                           const QStyleOptionViewItem& option,
                                           const QModelIndex& index) {
  auto visBtnPos = getVisibleBtnRect(option.rect);
  auto lockBtnPos = visBtnPos.translated(-visButtonLength - 5, 0);

  switch (event->type()) {
    case QEvent::MouseButtonPress: {
      auto mouseEvent = static_cast<QMouseEvent*>(event);
      if (visBtnPos.contains(mouseEvent->pos())) {
        emit itemVisiblePressed(index);
      }
      if (lockBtnPos.contains(mouseEvent->pos())) {
        emit itemLockPressed(index);
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
  this->setEditTriggers(EditTrigger::NoEditTriggers);
  this->setHeaderHidden(true);
  this->setSelectionMode(SelectionMode::ContiguousSelection);

  auto styleDelegate = new ItemStyleDelegate(this);
  this->setItemDelegate(styleDelegate);

  connect(styleDelegate, &ItemStyleDelegate::itemVisiblePressed, this,
          &LayerTreeView::handleItemVisiblePressed);
  connect(styleDelegate, &ItemStyleDelegate::itemVisibleMoved, this,
          &LayerTreeView::handleItemVisibleMoved);

  connect(styleDelegate, &ItemStyleDelegate::itemLockPressed, this,
          &LayerTreeView::handleItemLockPressed);
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

void views::LayerTreeView::handleItemVisiblePressed(const QModelIndex& index) {
  this->inVisibleDrag = true;
  cacheVisibleChangedIndex.push_back(index);

  auto visible = index.data(WaifuL2d::Layer::DataRole::LayerVisible).toBool();
  emit shouldSetVisible(index, !visible, true);
}

void views::LayerTreeView::handleItemVisibleMoved(const QModelIndex& index) {
  auto findItem = std::find(cacheVisibleChangedIndex.begin(),
                            cacheVisibleChangedIndex.end(), index);
  if (findItem != cacheVisibleChangedIndex.end()) {
    return;
  }

  auto visible = index.data(WaifuL2d::Layer::DataRole::LayerVisible).toBool();
  emit shouldSetVisible(index, !visible, false);
  cacheVisibleChangedIndex.push_back(index);
}

void views::LayerTreeView::handleItemLockPressed(const QModelIndex& index) {
  auto isLocked = index.data(WaifuL2d::Layer::DataRole::LayerLock).toBool();
  emit shouldSetLock(index, !isLocked);
}
