//
// Created by chen_yichen on 2024/7/30.
//
#include "model/layer.h"

#include "model/id_allocation.h"
namespace ProjectModel {
Layer::Layer(const QString &name, bool isVisible) {
  this->name = name;
  this->id = IdAllocation::getInstance().allocate();
  this->isvisible = isVisible;
}
const QString &Layer::getName() const { return name; }
int Layer::getId() const { return id; }
bool Layer::isVisible() const { return isvisible; }
Layer::~Layer() { IdAllocation::getInstance().release(this->id); }

}  // namespace ProjectModel
