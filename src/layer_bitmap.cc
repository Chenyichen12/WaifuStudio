//
// Created by chenyichen on 7/31/24.
//

#include "include/layer_bitmap.h"
namespace ProjectModel {
BitmapManager::BitmapManager(QObject *parent) : QObject(parent) {
  this->assetsMap = {};
}
void BitmapManager::addAsset(LayerBitmap *map) {
  this->assetsMap.insert({map->ID(), map});
}
void BitmapManager::deleteAsset(LayerBitmap *map) {
  if (this->assetsMap.contains(map->ID())) {
    this->assetsMap.erase(map->ID());
    delete map;
  }
}
const LayerBitmap *BitmapManager::getBitmap(int id) const {
  if (this->assetsMap.contains(id)) {
    return this->assetsMap.at(id);
  }
  return nullptr;
}
BitmapManager::~BitmapManager() {
  for (const auto &item : this->assetsMap) {
    delete item.second;
  }
}
}  // namespace ProjectModel
