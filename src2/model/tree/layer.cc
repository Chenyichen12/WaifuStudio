#include "layer.h"

namespace Tree {
int Layer::getId() const { return data(IdRole).toInt(); }

Layer::Layer(int id) : QStandardItem() {
  setData(IdRole, id);
  setData(VisibleRole, true);
}

bool Layer::getVisible() const { return data(VisibleRole).toBool(); }
void Layer::setVisible(bool visible) { setData(VisibleRole, visible); }

int Layer::type() const { return UserType + 1; }
}  // namespace Tree