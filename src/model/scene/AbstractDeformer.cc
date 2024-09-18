#include "AbstractDeformer.h"
#include "deformmanager.h"
namespace WaifuL2d {
DeformManager* AbstractDeformer::getManager() {
  auto parent = this->parentItem();
  while (parent) {
    auto type = parent->type();
    if(type == ManagerType) {
      return static_cast<DeformManager*>(parent);
    }
    parent = parent->parentItem();
  }
  return nullptr;
}

void AbstractDeformer::setMopherParent(AbstractDeformer* parent) {
  if (morpherParent) {
    morpherParent->morpherChildren.removeOne(this);
  }

  morpherParent = parent;
  if (parent) {
    parent->morpherChildren.append(this);
  }
}
AbstractDeformer::AbstractDeformer(QGraphicsItem* parent) : QGraphicsItem(parent) {
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}
}  // namespace WaifuL2d
