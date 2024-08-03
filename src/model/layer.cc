//
// Created by chen_yichen on 2024/7/30.
//
#include "layer.h"

namespace ProjectModel {

Layer::Layer(int id) {
  QStandardItem::setData(id, UserIdRole);
  QStandardItem::setData(true, VisibleRole);
}

void Layer::setData(const QVariant& value, int role) {
  if (role == UserIdRole) {
    return;
  }
  QStandardItem::setData(value, role);
}

int Layer::getId() const { return this->data(UserIdRole).toInt(); }

bool Layer::getVisible() const { return this->data(VisibleRole).toBool(); }

void Layer::setVisible(bool vis) { this->setData(vis, VisibleRole); }

BitmapLayer::BitmapLayer(int id, int bitmapId) : Layer(id) {
  this->bitmapId = bitmapId;
}

PsGroupLayer::PsGroupLayer(int id) : Layer(id) {}
}  // namespace ProjectModel
