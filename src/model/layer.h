//
// Created by chen_yichen on 2024/7/30.
//

#pragma once
#include <QStandardItem>

namespace ProjectModel {

enum LayerTypes {
  BitmapLayerType = QStandardItem::UserType + 1,
  PsGroupLayerType = QStandardItem::UserType + 2,
};
constexpr int UserIdRole = Qt::UserRole + 1;
constexpr int VisibleRole = Qt::UserRole + 2;
class Layer : public QStandardItem {
 protected:
  bool visible;

 public:
  int type() const override = 0;
  Layer(int id);
  void setData(const QVariant& value, int role) override;
  int getId() const;

  bool getVisible() const;
  void setVisible(bool);
};

class BitmapLayer : public Layer {
 public:
  int type() const override { return LayerTypes::BitmapLayerType; }
  int getBitmapId() const { return bitmapId; }
  explicit BitmapLayer(int id, int bitmapId);

 private:
  int bitmapId;
};

class PsGroupLayer : public Layer {
 public:
  explicit PsGroupLayer(int id);
  int type() const override { return LayerTypes::PsGroupLayerType; }
};
}  // namespace ProjectModel
