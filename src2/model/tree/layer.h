#pragma once
#include <QStandardItem>
namespace Tree {

constexpr int IdRole = Qt::UserRole + 1;
constexpr int VisibleRole = Qt::UserRole + 2;

class Layer : public QStandardItem {
 public:
  Layer(int id);
  virtual int getId() const;
  bool getVisible() const;
  void setVisible(bool visible);
  int type() const override;
};
}  // namespace Tree