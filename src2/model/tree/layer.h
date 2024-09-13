#pragma once
#include <QStandardItem>
namespace Tree {
class Layer : public QStandardItem {
 private:
  int id;

 public:
  enum Type {
    BitmapLayer = UserType + 1,
    GroupLayer = UserType + 2,
    MeshMopherLayer = UserType + 3,
    RotationMopherLayer = UserType + 4,
    BezierMopherLayer = UserType + 5
  };

  enum DataRole {
    LayerVisible = Qt::UserRole + 1,
    LayerLock = Qt::UserRole + 2,
  };

  Layer(int id);
  Layer(const QString& name, int id);
  int getId() const;
  int type() const override = 0;

  QString getName() const;
  void setName(const QString& name);

  bool getVisible() const;
  void setVisible(bool visible);

  bool getLocked() const;
  void setLocked(bool locked);
};

}  // namespace Tree