#pragma once
#include <QObject>

#include "layer.h"
#include "layer_bitmap.h"

namespace ProjectModel {
class TreeManager;
class LayerModel : public QObject {
 private:
  TreeManager* psdTreeManager;
  TreeManager* controllerTreeManger;

 public:
  LayerModel(TreeManager* psdTreeManager, TreeManager* controllerTreeManger,
             QObject* parent = nullptr);

  TreeManager* getPsdTreeManager() const;
  TreeManager* getControllerTreeManger() const;
};

class BitmapLayer : public Layer {
 private:
  const LayerBitmap& bitmap;

 public:
  BitmapLayer(const QString& name, const LayerBitmap& bitmap,
              bool isVisible = false);

 public:
  bool isContainer() override;
  inline const LayerBitmap& getBitMap() { return bitmap; };
};

class PsGroupLayer : public Layer {
 public:
  inline bool isContainer() override { return true; }
  explicit PsGroupLayer(const QString& name, bool isVisible = true)
      : Layer(name, isVisible) {}
};
}  // namespace ProjectModel
