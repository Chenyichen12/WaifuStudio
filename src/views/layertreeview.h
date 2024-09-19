#pragma once

#include <QStyledItemDelegate>
#include <QTreeView>
// QT_BEGIN_NAMESPACE
namespace views {
class LayerTreeView : public QTreeView {
  Q_OBJECT
 private:
  QModelIndexList cacheVisibleChangedIndex = {};
  bool inVisibleDrag = false;

 public:
  LayerTreeView(QWidget* parent = nullptr);

 protected:
  void mouseReleaseEvent(QMouseEvent* event) override;
  void startDrag(Qt::DropActions supportedActions) override;
 signals:
  void shouldSetVisible(const QModelIndex& index, bool isVisible, bool isStart);
  void shouldSetLock(const QModelIndex& index, bool isLocked);
 protected:
  void handleItemVisiblePressed(const QModelIndex&);
  void handleItemVisibleMoved(const QModelIndex&);
  void handleItemLockPressed(const QModelIndex&);
};
class ItemStyleDelegate : public QStyledItemDelegate {
  Q_OBJECT
 private:
  QImage visibleImg;
  QImage invisibleImg;

  QImage lockImg;
  int visButtonLength;

 protected:
  bool editorEvent(QEvent* event, QAbstractItemModel* model,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) override;

  QRect getVisibleBtnRect(const QRect& relative) const;


 public:
  void paint(QPainter* painter, const QStyleOptionViewItem& option,
             const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& option,
                 const QModelIndex& index) const override;

  ItemStyleDelegate(QObject* parent = nullptr);

  static bool isLightColor(const QColor& color);
 signals:
  void itemVisiblePressed(const QModelIndex&);
  void itemVisibleMoved(const QModelIndex&);

  void itemLockPressed(const QModelIndex&);
};

}  // namespace views
// QT_END_NAMESPACE
