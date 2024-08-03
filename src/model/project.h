//
// Created by chenyichen on 7/31/24.
//
#pragma once
#include "QObject"
namespace ProjectModel{
class LayerModel;
class BitmapManager;
class Project;

class ProjectBuilder {
 private:
  BitmapManager* manager = nullptr;
  LayerModel* model = nullptr;

 public:
  Project* build() const;
  void setBitmapManager(BitmapManager*);
  void setLayerModel(LayerModel*);
  ProjectBuilder() = default;
};
class Project : public QObject {
  friend ProjectBuilder;
 private:
  Project() = default;
  BitmapManager* bitmapManager = nullptr;
  LayerModel* layerModel = nullptr;
 public:
  BitmapManager* getBitmapManager() const { return bitmapManager; }
  LayerModel* getLayerModel() const { return layerModel; }
};


}

