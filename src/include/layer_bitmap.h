//
// Created by chenyichen on 7/31/24.
//

#pragma once
#include "QObject"

namespace ProjectModel{
struct LayerBitmap {
 private:
  int id;

 public:
  LayerBitmap(const LayerBitmap&) = delete;
  explicit LayerBitmap(int id) { this->id = id; };
  inline int ID() const { return id; }
  int top = 0;
  int left = 0;
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned char* rawBytes = nullptr;

  ~LayerBitmap() { delete[] rawBytes; }
};

class BitmapManager : public QObject {
 protected:
  std::unordered_map<int, LayerBitmap*> assetsMap;

 public:
  explicit BitmapManager(QObject* parent = nullptr);
  BitmapManager(const BitmapManager&) = delete;
  ~BitmapManager();
  void addAsset(LayerBitmap* map);
  void deleteAsset(LayerBitmap* map);
  const LayerBitmap* getBitmap(int id) const;
};
}

