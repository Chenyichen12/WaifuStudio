#pragma once
#include <QStandardItem>
namespace WaifuL2d {
class Layer : public QStandardItem {
 public:
#define TYPE_ENUM_LIST                 \
  X(BitmapLayer, UserType + 1)         \
  X(GroupLayer, UserType + 2)          \
  X(MeshMopherLayer, UserType + 3)     \
  X(RotationMopherLayer, UserType + 4) \
  X(BezierMopherLayer, UserType + 5)

  enum Type {
#define X(EnumName, EnumValue) EnumName = EnumValue,
    TYPE_ENUM_LIST
#undef X
  };

  static bool isLayer(int type) {
    switch (type) {
#define X(EnumName, EnumValue) \
  case EnumValue:              \
    return true;
      TYPE_ENUM_LIST
#undef X
      default:
        return false;
    }
  };
#undef TYPE_ENUM_LIST

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