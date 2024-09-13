#include "layer.h"

namespace Tree {
Layer::Layer(int id) {
  this->id = id;
  setVisible(true);
  setLocked(false);
}

Layer::Layer(const QString& name, int id) : Layer(id) {
  setText(name);
  this->id = id;
}
QString Layer::getName() const { return text(); }

int Layer::getId() const { return id; }

void Layer::setName(const QString& name) { setText(name); }

bool Layer::getVisible() const {
  return data(Layer::DataRole::LayerVisible).toBool();
}

void Layer::setVisible(bool visible) {
  setData(visible, Layer::DataRole::LayerVisible);
}

bool Layer::getLocked() const {
  return data(Layer::DataRole::LayerLock).toBool();
}

void Layer::setLocked(bool locked) {
  setData(locked, Layer::DataRole::LayerLock);
}

}  // namespace Tree