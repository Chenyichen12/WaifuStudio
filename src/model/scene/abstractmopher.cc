#include "abstractmopher.h"
#include "mophermanager.h"
namespace WaifuL2d {
MopherManager* AbstractMopher::getManager() {
  auto parent = this->parentItem();
  while (parent) {
    auto type = parent->type();
    if(type == ManagerType) {
      return static_cast<MopherManager*>(parent);
    }
    parent = parent->parentItem();
  }
  return nullptr;
}

void AbstractMopher::setMopherParent(AbstractMopher* parent) {
  if (morpherParent) {
    morpherParent->morpherChildren.removeOne(this);
  }

  morpherParent = parent;
  if (parent) {
    parent->morpherChildren.append(this);
  }
}
AbstractMopher::AbstractMopher(QGraphicsItem* parent) : QGraphicsItem(parent) {
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}
}  // namespace WaifuL2d
