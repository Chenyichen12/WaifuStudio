//
// Created by chen_yichen on 2024/7/30.
//
#include "include/layer.h"

Layer::Layer(const QString &name, int id, bool isVisible) {
  this->name = name;
  this->id = id;
  this->isvisible = isVisible;
}
const QString &Layer::getName() const { return name; }
int Layer::getId() const { return id; }
bool Layer::isVisible() const { return isvisible; }
