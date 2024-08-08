//
// Created by chenyichen on 7/31/24.
//

#pragma once
#include "QObject"

namespace ProjectModel{
struct BitmapAsset {
 private:
  int id;

 public:
  BitmapAsset(const BitmapAsset&) = delete;
  explicit BitmapAsset(int id) { this->id = id; };
  inline int ID() const { return id; }
  int top = 0;
  int left = 0;
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned char* rawBytes = nullptr;

  ~BitmapAsset() { delete[] rawBytes; }
};

class BitmapManager : public QObject {
 protected:
  std::unordered_map<int, BitmapAsset*> assetsMap;

 public:
  explicit BitmapManager(QObject* parent = nullptr);
  BitmapManager(const BitmapManager&) = delete;
  ~BitmapManager() override;
  void addAsset(BitmapAsset* map);
  void deleteAsset(BitmapAsset* map);
  BitmapAsset* getBitmap(int id) const;
};
}

