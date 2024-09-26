#pragma once
#include <QWidget>

namespace views {
class IconButton : public QWidget {
  Q_OBJECT
  QImage icon;
  bool selected;

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

public:
  IconButton(QWidget* parent = nullptr);
  void setIcon(const QString& iconPath);
  void setSelect(bool selected);
signals:
  void clicked();
};
}