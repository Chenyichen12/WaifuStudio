#include "abstractdeformer.h"

namespace WaifuL2d {
void AbstractDeformer::setDeformerParent(AbstractDeformer* parent) {
  if (deformerParent) {
    deformerParent->deformerChildren.removeOne(this);
  }

  deformerParent = parent;
  if (parent) {
    parent->deformerChildren.append(this);
  }
}

AbstractDeformer::AbstractDeformer(QGraphicsItem* parent)
  : QGraphicsItem(parent) {
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}
} // namespace WaifuL2d