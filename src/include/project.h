//
// Created by chenyichen on 7/31/24.
//
#pragma once
#include "QObject"
namespace ProjectModel{
class LayerModel;
class BitmapManager;

class ProjectBuilder;
class Project : public QObject {
  Q_OBJECT
  friend ProjectBuilder;

 private:
  BitmapManager* bitmapManager = nullptr;
  LayerModel* layerModel = nullptr;
  Project() = default;

 public:
  BitmapManager* getBitmapManager() const { return bitmapManager; }
  LayerModel* getLayerModel() const { return layerModel; }
};

class ProjectBuilder {
 private:
  BitmapManager* manager = nullptr;
  LayerModel* model = nullptr;

 public:
  Project* build();
  void setBitmapManager(BitmapManager*);
  void setLayerModel(LayerModel*);
  ProjectBuilder() = default;
};
}

