#include "layer.h"

namespace WaifuL2d {
Layer::Layer() {
  setVisible(true);
  setLocked(false);
}

Layer::Layer(const QString& name) : Layer() { setText(name); }
QString Layer::getName() const { return text(); }

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
  setSelectable(!locked);
  setData(locked, Layer::DataRole::LayerLock);
}
void Layer::setId(int id) { setData(id, DataRole::LayerId); }
int Layer::getId() const { return data(DataRole::LayerId).toInt(); }
BitmapLayer::BitmapLayer(const QString& name, const QImage& image)
    : Layer(name) {
  auto img = image.scaledToHeight(25);
  setIcon(QIcon(QPixmap::fromImage(img)));
  this->setDropEnabled(false);
}

GroupLayer::GroupLayer(const QString& name) : Layer(name) {}

}  // namespace WaifuL2d