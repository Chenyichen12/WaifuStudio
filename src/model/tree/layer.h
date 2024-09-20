#pragma once
#include <QStandardItem>
namespace WaifuL2d {
class Layer : public QStandardItem {
 public:
  enum Type {
    BitmapLayer = UserType + 1,
    GroupLayer = UserType + 2,
    MeshMopherLayer = UserType + 3,
    RotationMopherLayer = UserType + 4,
    BezierMopherLayer = UserType + 5
  };

  enum DataRole {
    LayerId = Qt::UserRole + 1,
    LayerVisible = Qt::UserRole + 2,
    LayerLock = Qt::UserRole + 3,
  };

  Layer();
  Layer(const QString& name);
  int type() const override = 0;

  QString getName() const;
  void setName(const QString& name);

  bool getVisible() const;
  void setVisible(bool visible);

  bool getLocked() const;
  void setLocked(bool locked);

  void setId(int id);
  int getId() const;
};

class BitmapLayer : public Layer {
 public:
  BitmapLayer(const QString& name, const QImage& image);
  int type() const override { return Type::BitmapLayer; };
};

class GroupLayer : public Layer {
 public:
  GroupLayer(const QString& name);
  int type() const override { return Type::GroupLayer; };
};
}  // namespace WaifuL2d