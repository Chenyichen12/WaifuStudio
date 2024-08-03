#pragma once

#include <QStyledItemDelegate>
#include <QTreeView>
QT_BEGIN_NAMESPACE

namespace views {

class ItemStyleDelegate : public QStyledItemDelegate {
 private:
  QImage visibleImg;
  QImage invisibleImg;

 public:
  void paint(QPainter* painter, const QStyleOptionViewItem& option,
             const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& option,
                 const QModelIndex& index) const override;

  ItemStyleDelegate(QObject* parent = nullptr);
};
class LayerTreeView : public QTreeView {
 public:
  LayerTreeView(QWidget* parent = nullptr);
};
}  // namespace views

QT_END_NAMESPACE
