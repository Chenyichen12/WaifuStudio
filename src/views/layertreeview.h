#pragma once

#include <QStyledItemDelegate>
#include <QTreeView>
// QT_BEGIN_NAMESPACE
namespace views {
class LayerTreeView : public QTreeView {
  Q_OBJECT
 private:
  std::list<QModelIndex> cacheVisibleChangedIndex = {};
  bool inVisibleDrag = false;

 public:
  LayerTreeView(QWidget* parent = nullptr);

 protected:
  void mouseReleaseEvent(QMouseEvent* event) override;
  void startDrag(Qt::DropActions supportedActions) override;
signals:
  void shouldSetVisible(const QModelIndex& index, bool isVisible);
 protected:
  void handleItemVisiblePressed(const QModelIndex&);
  void handleItemVisibleMoved(const QModelIndex&);
};
class ItemStyleDelegate : public QStyledItemDelegate {
  Q_OBJECT
 private:
  QImage visibleImg;
  QImage invisibleImg;
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

 signals:
  void itemVisiblePressed(const QModelIndex&);
  void itemVisibleMoved(const QModelIndex&);
};

}  // namespace views
// QT_END_NAMESPACE
